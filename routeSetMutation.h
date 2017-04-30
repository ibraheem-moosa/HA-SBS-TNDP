/* 
 * File:   routeSetMutation.h
 * Author: MAN
 *
 * Created on September 8, 2013, 9:02 PM
 */

#ifndef _ROUTESETMUTATION_H
#define	_ROUTESETMUTATION_H

#define FRONT 0
#define BACK 1

//#include <eoOp.h>
//#include<utils/eoRNG.h>
#include <algorithm>
#include "route.h"
#include "inputData.h"
//#include "./heuristics/genRoute.h"

template<class GenotypeT>
int rouletteWheelForRoute(GenotypeT & _genotype, eoEvalFuncPtr< Route<double> >& _eval)
{
    std::vector <double> fits;
    double total = 0;
    for (int i = 0; i < _genotype.size(); i++)
    {
        _eval(_genotype[i]);
        total += _genotype[i].fitness();
        fits.push_back(_genotype[i].fitness());
    }
    return rng.roulette_wheel(fits, total);

}

int rouletteWheelForPath(int startIndex)
{
    std::vector <double> fits;
    double total = 0;
    for (int i = 0; i < VERTICES_NO; i++)
    {
        if (startIndex == i)
        {
            fits.push_back(0);
            continue;
        }
        total += DS[startIndex][i];
        fits.push_back(DS[startIndex][i]);
    }
    return rng.roulette_wheel(fits, total);
}
int rouletteWheelForNode( int startIndex,std::vector<int> AdjList, int mutIndex)
{
    if(AdjList.size() == 1)
        return 0;
    std::vector <double> fits;
    double total = 0;
    double fit;
    for(int i=0;i<AdjList.size();i++)
    {
            fit = 1/( 1+p[mutIndex][startIndex][AdjList[i]] );    
            fits.push_back(fit);
            total += fit;
    }
    return rng.roulette_wheel(fits, total);
}

template<class GenotypeT>
class SmallMutation : public eoMonOp<GenotypeT>
{
public:
    /**
     * Ctor - no requirement
     */
    // START eventually add or modify the anyVariable argument

    SmallMutation(float _pDelete, eoEvalFuncPtr< Route<double> >& _eval) : pDelete(_pDelete), eval(_eval)
    {
        // START Code of Ctor of an SmallMutation object
        // END   Code of Ctor of an SmallMutation object
    }

    /// The class name. Used to display statistics

    std::string className() const
    {
        return "SmallMutation";
    }

    /**
     * modifies the parent
     * @param _genotype The parent genotype (will be modified)
     */
    bool operator()(GenotypeT & _genotype)
    {
        // START code for mutation of the _genotype object
        int mutIndex = rouletteWheelForRoute(_genotype, eval);
        Route<double>& mutRoute = _genotype[mutIndex];


        //  std::cout << mutRoute << std::endl;
        std::list<int>::iterator it[2], nIt;
        it[FRONT] = mutRoute.mutableR().begin();
        it[BACK] = mutRoute.mutableR().end();
        it[BACK]--; //points to last elem
        int selectedEnd = rng.flip();
        nIt = it[selectedEnd];
        int selectedNode = *it[selectedEnd];

        if (pDelete >= rng.uniform()) //delete a terminal
        {

            if (mutRoute.mutableR().size() <= parameters["minRouteLength"]){
                goto APPEND; 
            }
DELETE:     mutRoute.erase(it[selectedEnd]);
            mutRoute.nodeList[selectedNode] = 0;
        }
        else //append a terminal
        {
            
            if (mutRoute.mutableR().size() >= parameters["maxRouteLength"]){
                goto DELETE;
            }

APPEND:     vector<int> AdjListForSelected = AdjList[selectedNode];
            for (vector<int>::iterator iit = AdjListForSelected.begin(); iit != AdjListForSelected.end(); iit++)
            {
                if (mutRoute.nodeList[*iit] == 1)
                {
                    AdjListForSelected.erase(iit);
                    --iit;
                }
            }
            if (AdjListForSelected.size() == 0)
                return false;
            int neighbor;
            if (selectedEnd == FRONT)
            {
                nIt++;
                neighbor = *nIt;
            }
            else
            {
                nIt--;
                neighbor = *nIt;
                it[BACK]++; //again points to end
            }
            int toAddIndex;
           
           // toAddIndex = rng.random(AdjListForSelected.size());
            toAddIndex = rouletteWheelForNode(selectedNode,AdjListForSelected,mutIndex);
           
            mutRoute.insert(it[selectedEnd], AdjListForSelected[toAddIndex]);
            mutRoute.nodeList[AdjListForSelected[toAddIndex]] = 1;
        }

		assert(mutRoute.mutableR().size() >= minRouteSize);
		assert(mutRoute.mutableR().size() <= maxRouteSize);
 
       
               mutRoute.invalidate();
        return true;
        // END code for mutation of the _genotype object
    }

private:
    // START Private data of an SmallMutation object
    //  varType anyVariable;		   // for example ...
    // END   Private data of an SmallMutation object
    float pDelete;
    eoEvalFuncPtr< Route<double> >& eval;
};


template<class GenotypeT>
class RouteCrossMutation : public eoMonOp<GenotypeT>
{
public:
    /**
     * Ctor - no requirement
     */
    // START eventually add or modify the anyVariable argument

    RouteCrossMutation(eoEvalFuncPtr< Route<double> >& _eval, int minRouteSize, int maxRouteSize) 
	    : eval(_eval), minRouteSize(minRouteSize), maxRouteSize(maxRouteSize)
    //  BigMutation( varType  _anyVariable) : anyVariable(_anyVariable)
    // END eventually add or modify the anyVariable argument
    {

        // START Code of Ctor of an BigMutation object
        // END   Code of Ctor of an BigMutation object
    }

    /// The class name. Used to display statistics
    string className() const
    {
        return "RouteCrossMutation";
    }

    /**
     * modifies the parent
     * @param _genotype The parent genotype (will be modified)
     */
    bool operator()(GenotypeT & _genotype)
    {
			// START code for mutation of the _genotype object
		// Randomly select two routes
		int try_count = 0;
SELECT_ROUTES:	
		int r1 = random() % _genotype.size();
		int r2 = random() % _genotype.size();
		if(try_count == 10)
			return false;
		try_count++;
		Route<double>& route1 = _genotype[r1];
		Route<double>& route2 = _genotype[r2];
		vector<int> common_nodes;
		for(int i = 0; i < route1.nodeList.size(); i++)
		{
			if(route1.nodeList[i] != 0 && route2.nodeList[i] != 0)
			{
				common_nodes.push_back(i);
			}
		}
		if(common_nodes.size() == 0)
			goto SELECT_ROUTES;
		
		for(int i = 0; i < common_nodes.size(); i++)
		{
			//int random_common_node = common_nodes[random() % common_nodes.size()];
			std::list<int>& r1_list = route1.mutableR();
			std::list<int>& r2_list = route2.mutableR();
			std::list<int>::iterator r1_iter = std::find(r1_list.begin(), r1_list.end(), common_nodes[i]);
			std::list<int>::iterator r2_iter = std::find(r2_list.begin(), r2_list.end(), common_nodes[i]);
			std::list<int> new_r1_list;
			for(auto it = r1_list.begin(); it != r1_iter; it++)
			{
				new_r1_list.push_back(*it);
			}
			for(auto it = r2_iter; it != r2_list.end(); it++)
			{
				new_r1_list.push_back(*it);
			}


			std::list<int> new_r2_list;
			for(auto it = r2_list.begin(); it != r2_iter; it++)
			{
				new_r2_list.push_back(*it);
			}
			for(auto it = r1_iter; it != r1_list.end(); it++)
			{
				new_r2_list.push_back(*it);
			}
			bool invalid = false;
			vector<bool> occurence_check(VERTICES_NO, false);
			for(auto vertex : new_r1_list)
			{
				if(occurence_check[vertex]){
					invalid = true;
					break;
				}
				occurence_check[vertex] = true;
			}
			if(invalid)
				continue;
			for(int i = 0; i < VERTICES_NO; i++)
				occurence_check[i] = false;
			for(auto vertex : new_r2_list)
			{
				if(occurence_check[vertex]){
					invalid = true;
					break;
				}
				occurence_check[vertex] = true;
			}
			if(invalid)
				continue;
			if(new_r1_list.size() < minRouteSize || new_r1_list.size() > maxRouteSize)
				continue;
			if(new_r2_list.size() < minRouteSize || new_r2_list.size() > maxRouteSize)
				continue;


			route1.setR(new_r1_list);

			vector<int> newNodeList(VERTICES_NO, 0);
				for (list<int>::iterator lit = new_r1_list.begin(); lit != new_r1_list.end(); lit++)
				{
					newNodeList[*lit] = 1;
				}
				route1.setNodeList(newNodeList);
			route1.invalidate();

			route2.setR(new_r2_list);
			newNodeList = vector<int>(VERTICES_NO, 0);
				for (list<int>::iterator lit = new_r2_list.begin(); lit != new_r2_list.end(); lit++)
				{
					newNodeList[*lit] = 1;
				}
				route2.setNodeList(newNodeList);
				route2.invalidate();

			return true;
		}
		return false;
        // END code for mutation of the _genotype object
    }

private:
    // START Private data of an BigMutation object
    eoEvalFuncPtr< Route<double> >& eval; // END   Private data of an BigMutation object
    int minRouteSize, maxRouteSize;
};




template<class GenotypeT>
class BigMutation : public eoMonOp<GenotypeT>
{
public:
    /**
     * Ctor - no requirement
     */
    // START eventually add or modify the anyVariable argument

    BigMutation(eoEvalFuncPtr< Route<double> >& _eval) : eval(_eval)
    //  BigMutation( varType  _anyVariable) : anyVariable(_anyVariable)
    // END eventually add or modify the anyVariable argument
    {

        // START Code of Ctor of an BigMutation object
        // END   Code of Ctor of an BigMutation object
    }

    /// The class name. Used to display statistics
    string className() const
    {
        return "BigMutation";
    }

    /**
     * modifies the parent
     * @param _genotype The parent genotype (will be modified)
     */
    bool operator()(GenotypeT & _genotype)
    {
        // START code for mutation of the _genotype object
        int mutIndex = rouletteWheelForRoute(_genotype, eval);
        Route<double>& mutRoute = _genotype[mutIndex];

        //std::cout<<_genotype[mutIndex]<<std::endl;

		Matrix tempDemand(demand);
		
		for(int i=0; i<_genotype.size(); i++){
			if(i==mutIndex) continue;
			const std::list<int>& _chrom = _genotype[i].R();
			std::list<int>::const_iterator outer = _chrom.end();
			for (outer--; outer != _chrom.begin(); outer--)
			{
				std::list<int>::const_iterator inner;
				for (inner = _chrom.begin(); inner != outer; inner++)
				{
					tempDemand[*inner][*outer] = 0;
					tempDemand[*outer][*inner] = 0;
				}
			}

		}

		int from = (_genotype[mutIndex].R().front()), to = (_genotype[mutIndex].R().back());
		vector<int> result;
		vector<vector<int> > resultRouteSet; //will contain only one route
		
		getBestRouteSet(dist, tempDemand, 1, resultRouteSet, rng.uniform());
		assert(resultRouteSet[0].size() <= maxRouteSize);
		assert(resultRouteSet[0].size() >= minRouteSize);
		
		//cout << "NEW PATH FOUND\n";
		result = resultRouteSet[0];
		for(int j = 0; j < result.size() - 1; j++)
		{
		    assert(tr[result[j]][result[j + 1]] != INFINITY);
		}
		list<int> newPath;
		for(auto x : result) {
			newPath.push_back(x);
		}

        mutRoute.setR(newPath); ///work here
        vector<int> newNodeList(VERTICES_NO, 0);
        for (list<int>::iterator lit = newPath.begin(); lit != newPath.end(); lit++)
        {
            newNodeList[*lit] = 1;
        }
        mutRoute.setNodeList(newNodeList);

        mutRoute.invalidate();
        return true;
        // END code for mutation of the _genotype object
    }

private:
    // START Private data of an BigMutation object
    eoEvalFuncPtr< Route<double> >& eval; // END   Private data of an BigMutation object
};


#endif	/* _ROUTESETMUTATION_H */


