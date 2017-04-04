/* 
 * File:   adjustedEvalFunc.h
 * Author: MAN
 *
 * Created on June 18, 2014, 5:53 PM
 */

#ifndef ADJUSTEDEVALFUNC_H
#define	ADJUSTEDEVALFUNC_H

template <class EOT>
class AdjustedEvalFunc : public eoEvalFunc<EOT>
{
public:
    /// Ctor - no requirement
    // START eventually add or modify the anyVariable argument

    AdjustedEvalFunc(RouteSetEvalFunc<RouteSet<double > > eval)
    {
        // START Code of Ctor of an AdjustedEvalFunc object
        _actualEval = eval;
        // END   Code of Ctor of an AdjustedEvalFunc object
    }

    /** Actually compute the fitness
     *
     * @param EOT & _eo the EO object to evaluate
     *                  it should stay templatized to be usable
     *                  with any fitness type
     */
#pragma GCC push_options
#pragma GCC optimize("O3")
    void operator()(EOT & _eo)
    {
        if (_eo.invalid())
        {

            _actualEval(_eo);

            vector< Route<double> > & routeSet = _eo.mutableRs();
            double pSum = 0;
            list<int>::const_iterator next, it; 
            for (int r = 0; r < routeSet.size(); r++)
            {
                next = routeSet[r].R().begin();
                it = next++;
                for (; next != routeSet[r].R().end(); it++, next++)
                {
                    int i = *it;
                    int j = *next;
                    pSum = pSum + p[r][i][j];
                }
            }

            double adjustedFitness = _eo.actualFitness - parameters["beta"] * pSum;

            //cout << "actual fitness: " <<  _eo.actualFitness << "penalty: " << pSum << "weighted penalty: " << parameters["beta"] * pSum << endl; 
            _eo.fitness(adjustedFitness); //minimize
        }
    }
#pragma GCC pop_options
private:
    RouteSetEvalFunc<RouteSet<double > > _actualEval;
};

#endif	/* ADJUSTEDEVALFUNC_H */

