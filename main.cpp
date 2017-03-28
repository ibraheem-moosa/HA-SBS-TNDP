// standard includes
#include <iostream>
#include <string.h>

// the general include for eo
#include <eo>
#include "eoGAWE.h"
#include "eoFitnessImprovementRatioContinue.h"

using namespace std;

#include "routeSet.h"
#include "inputData.h"
#include "popInit.h"
#include "routeFitness.h"
#include "routeSetEvalFunc.h"
#include "routeSetQuadCrossover.h"
#include "routeSetMutation.h"
#include "adjustedEvalFunc.h"

typedef eoMinimizingFitness MyFitT;
typedef RouteSet<double> Indi;
double penalizability(int a, int b, int c);


ofstream fout("best.txt");
double avg[5], bestFitnessMetrics[5], allRunBestFitness;

void main_function(int argc, char **argv)
{
    rng.reseed(time(0));

    //fitness evaluators
    eoEvalFuncPtr< Route<double> > routeEval(routeFitness);
    RouteSetEvalFunc<Indi> routeSetEval;
    AdjustedEvalFunc<Indi> adjustedEval(routeSetEval);

    //population
    eoPop<Indi> pop;
    popInit(pop, parameters["popSize"], parameters["routeSetSize"], adjustedEval); //changed

    //selection
    eoDetTournamentSelect<Indi> selectOne(parameters["tFit"]);
    double perc = (parameters["popSize"] - parameters["eilte"]) / parameters["popSize"]; //eilte
    eoSelectPerc<Indi> select(selectOne, perc);

    //crossover
    RouteSetQuadCrossover<Indi> xover(parameters["pSwap"]);

    //mutations
    BigMutation<Indi> bM(routeEval);
    SmallMutation<Indi> sM(parameters["pDelete"], routeEval);
    eoPropCombinedMonOp<Indi> mutation(sM, parameters["pSm"]);
    mutation.add(bM, 1 - parameters["pSm"]);

    //stop after maxGen generations
    //eoGenContinue<Indi> continuator(parameters["minGen"], parameters[]);
    eoFitnessImprovementRatioContinue<Indi> continuator(10, 50, 100, 0.000);
    //CHECKPOINT
    eoCheckPoint<Indi> checkpoint(continuator);
    // Create a counter parameter
    eoValueParam<unsigned> generationCounter(0, "Generation");
    // Create an incrementor (sub-class of eoUpdater). Note that the
    // parameter's value is passed by reference,
    // so every time the incrementer is updated (every generation),
    // the data in generationCounter will change.
    eoIncrementor<unsigned> increment(generationCounter.value());
    // Add it to the checkpoint,
    // so the counter is updated (here, incremented) every generation
    checkpoint.add(increment);
    // The Stdout monitor will print parameters to the screen ...
    eoStdoutMonitor monitor(false);

    // when called by the checkpoint (i.e. at every generation)
    checkpoint.add(monitor);

    // the monitor will output a series of parameters: add them
    monitor.add(generationCounter);
    //THE ALGORITHM
    eoGAWE<Indi> gga(select, xover, mutation,
            adjustedEval, checkpoint, parameters["elite"]); //changed
    // cout << pop[0];
    gga.best = pop[0];
    gga.actualBest = pop[0];
    gga.initial = pop[0];
    gga.stableCount = parameters["stableCount"];
    gga.minFitnessRatio = parameters["minFitnessRatio"];
    cout << "Initial : " << gga.best.fitness() << endl << gga.best.D[0] << endl << gga.best.D[1] << endl << gga.best.D[2] << endl << gga.best.Dun << endl << gga.best.ATT << endl;
    for (int i = 0; i < parameters["maxIter"]; i++)
    {
        cout << endl << "-----------------------------------------" << endl << "Iteration:" << i << " started" << endl << endl;

        gga.genCount = parameters["maxGen"];
        gga(pop);

        vector< Route<double> > & routeSet = gga.best.mutableRs();
        for (int r = 0; r < routeSet.size(); r++)
        {
            double maxPenalizibility = -1;
            double minPenalizibility = INFINITY;
            vector<edge> edgeList;

            list<int>::const_iterator next = routeSet[r].R().begin();
            list<int>::const_iterator it = next++;
            for (; next != routeSet[r].R().end(); it++, next++)
            {
                int i = *it;
                int j = *next;

                double penalty = penalizability(i, j, r);
                if (penalty > maxPenalizibility) maxPenalizibility = penalty;
                if (penalty < minPenalizibility) minPenalizibility = penalty;

                edgeList.push_back(edge(i, j, penalty));
            }

            //double randomPenalty = rng.uniform(minPenalizibility,maxPenalizibility);
            for (int i = 0; i < edgeList.size(); i++)
            {
                if ( (edgeList[i].penalizibility-minPenalizibility)/(maxPenalizibility-minPenalizibility) > 0.8)
                {
                    int a = edgeList[i].a, b = edgeList[i].b;
                    p[r][a][b]++;
                    p[r][b][a]++;
                }
            }


            for (int i = 0; i < VERTICES_NO; i++)
            {
                for (int j = i + 1; j < VERTICES_NO; j++)
                {
                    p[r][j][i] = p[r][i][j] = (1-parameters["e"])*p[r][i][j];
                }
            }
        }


        for (int i = 0; i < pop.size(); i++)
        {
            pop[i].invalidate();
            adjustedEval(pop[i]);
        }
    }
    cout << "FINAL Population size\n" << pop.size() << endl;
    cout << "Best : " << gga.actualBest << endl << gga.actualBest.D[0] << endl << gga.actualBest.D[1] << endl << gga.actualBest.D[2] << endl << gga.actualBest.Dun << endl << gga.actualBest.ATT << endl << gga.actualBest.fitness() << endl;

    fout << "Best : " << gga.actualBest << endl << gga.actualBest.D[0] << endl << gga.actualBest.D[1] << endl << gga.actualBest.D[2] << endl << gga.actualBest.Dun << endl << gga.actualBest.ATT << endl <<gga.actualBest.fitness()  << endl;
    avg[0] += gga.actualBest.D[0];
    avg[1] += gga.actualBest.D[1];
    avg[2] += gga.actualBest.D[2];
    avg[3] += gga.actualBest.Dun;
    avg[4] += gga.actualBest.ATT;

	if(gga.actualBest.fitness() > allRunBestFitness){
		allRunBestFitness = gga.actualBest.fitness();
		bestFitnessMetrics[0] = gga.actualBest.D[0];
		bestFitnessMetrics[1] = gga.actualBest.D[1];
		bestFitnessMetrics[2] = gga.actualBest.D[2];
		bestFitnessMetrics[3] = gga.actualBest.Dun;
		bestFitnessMetrics[4] = gga.actualBest.ATT;
	}

}

int main(int argc, char** argv)
{
	allRunBestFitness = -INFINITY;
    gatherAllInfo();
    //    createInitialSolution();
    //    exit(0);

    for (int j = 0; j < parameters["run"]; j++)
    {
        fout << "Run:" << (j + 1) << endl;
        try
        {
            main_function(argc, argv);
        }
        catch (exception& e)
        {
            cout << "Exception: " << e.what() << '\n';
        }

        fout << "Current Avg D0:" << avg[0] / (j + 1);
        fout << "\n\n" << endl;
    }
    fout << "Avg D0:" << avg[0] / parameters["run"] << endl << "Avg D1:" << avg[1] / parameters["run"] << endl << "Avg D2:" << avg[2] / parameters["run"] << endl << "Avg Dun:" << avg[3] / parameters["run"] << endl << "Avg ATT:" << avg[4] / parameters["run"] << endl;
    fout.close();

	cout << "All Run Best : " << bestFitnessMetrics[0] << endl << bestFitnessMetrics[1] << endl << bestFitnessMetrics[2] << endl << bestFitnessMetrics[3] << endl << bestFitnessMetrics[4] << endl << allRunBestFitness << endl;

	return (EXIT_SUCCESS);
}

double penalizability(int a, int b, int r)
{
    return 1.0 * tr[a][b] / ((1 + p[r][a][b]) * (1 + d[a][b])); //* tr[a][b]
}
