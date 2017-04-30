/* 
 * File:   routeSetQuadCrossOver.h
 * Author: MAN
 *
 * Created on September 8, 2013, 8:36 AM
 */

#ifndef _ROUTESETQUADCROSSOVER_H
#define	_ROUTESETQUADCROSSOVER_H

//#include <eoOp.h>

//#include "routeSet.h"

/**
 *  Always write a comment in this format before class definition
 *  if you want the class to be documented by Doxygen
 *
 * THere is NO ASSUMPTION on the class GenoypeT.
 * In particular, it does not need to derive from EO
 */
template<class GenotypeT>
class RouteSetQuadCrossover : public eoQuadOp<GenotypeT>
{
	public:
		/**
		 * Ctor - no requirement
		 */
		// START eventually add or modify the anyVariable argument

		RouteSetQuadCrossover(float _pSwap) : pSwap(_pSwap)
											  //  RouteSetQuadCrossover( varType  _anyVariable) : anyVariable(_anyVariable)
											  // END eventually add or modify the anyVariable argument
	{
		// START Code of Ctor of an eoOneMaxEvalFunc object
		// END   Code of Ctor of an eoOneMaxEvalFunc object
	}

		/// The class name. Used to display statistics

		std::string className() const
		{
			return "RouteSetQuadCross0ver";
		}

		/**
		 * eoQuad crossover - modifies both parents
		 * @param _genotype1 The first parent
		 * @param _genotype2 The second parent
		 */
		/*
		int routeSimilarity(Route<double> r1, Route<double> r2)
		{
			const list<int> l1 = r1.R();
			const list<int> l2 = r2.R();
			int numOfCommonNodes = 0;
			for(auto it1 = l1.begin(); it1 != prev(l1.end()); it1++)
			//for(int n1 : l1)
			{
				int from = *it1;
				int to = *(next(it1));
				bool found = false;
				for(auto it2 = l2.begin(); it2 != prev(l2.end()); it2++)
				//for(int n2 : l2)
				{
					if(from == *it2 && to == *(next(it2))) {
						found = true;
						break;
					}
				}
				if(found)
					numOfCommonNodes++;
			}
			return numOfCommonNodes;
		}
		*/
		int routeSimilarity(Route<double> r1, Route<double> r2)
		{
			const list<int> l1 = r1.R();
			const list<int> l2 = r2.R();
			int numOfCommonNodes = 0;
			for(int n1 : l1)
			{
				bool found = false;
				for(int n2 : l2)
				{
					if(n1 == n2) {
						found = true;
						break;
					}
				}
				if(found)
					numOfCommonNodes++;
			}
			return numOfCommonNodes;
		}
		
		bool operator()(GenotypeT& _genotype1, GenotypeT& _genotype2)
		{
			bool modified(false);
			// bool bad2 = _genotype2.isTrouble();
			// START code for crossover of _genotype1 and _genotype2 objects
			vector<bool> in_2_from_1(_genotype2.size(), false);     
			//those routes in 2 that have come from 1 after being swapped in this crossover

			for (int i = 0; i < _genotype1.size(); i++)
			{
				if (pSwap >= rng.uniform())
				{
					// now look for most similar route in 2
					int maxRouteSimilarity = 0;
					int mostSimilarRoute = 0;
					vector<int> route_similarities;
					vector<int> possible_routes;
					vector<int> most_similar_routes;
					for(int j = 0; j < _genotype2.size(); j++)
					{
						if(in_2_from_1[j]) continue; //this one came from 1, no need to swap back
						int s = routeSimilarity(_genotype1[i], _genotype2[j]);
						
						if(s == _genotype1[i].size())
							continue;
						route_similarities.push_back(s);
						possible_routes.push_back(j);
						if(s > maxRouteSimilarity)
						{
							maxRouteSimilarity = s;
						}
					}
					for(int j = 0; j < route_similarities.size(); j++)
					{
						if(route_similarities[j] == maxRouteSimilarity)
							most_similar_routes.push_back(possible_routes[j]);
					}
					mostSimilarRoute = most_similar_routes[random() % most_similar_routes.size()];
					printf("%d %d %d\n", _genotype1[i].size(), maxRouteSimilarity, most_similar_routes.size());
					/*
					if(_genotype1[i].size() - 1 == maxRouteSimilarity)
					{
						for(auto tmp: _genotype1[i].R())
						{
							printf("%d ", tmp);	
						}
						puts("");
						for(auto tmp: _genotype2[mostSimilarRoute].R())
						{
							printf("%d ", tmp);
						}
						puts("");
					}
					*/
					Route<double> temp = _genotype1[i];
					_genotype1[i] = _genotype2[mostSimilarRoute];
					_genotype2[mostSimilarRoute] = temp;
					modified = true;
					in_2_from_1[mostSimilarRoute] = true;
					//printf("%lf\n", (double)maxRouteSimilarity / _genotype1[i].size());
				}
			}
			/** Requirement
			 * if (at least one genotype has been modified) // no way to distinguish
			 *     oneAtLeastIsModified = true;
			 * else
			 *     oneAtLeastIsModified = false;
			 */
			//        if(bad != _genotype1.isTrouble() || bad2 != _genotype2.isTrouble())
			//        {
			//            std::cout<<"bad/n";
			//        }
			return modified;
			// END code for crossover of _genotype1 and _genotype2 objects
		}

	private:
		// START Private data of an RouteSetQuadCrossover object
		//  varType anyVariable;		   // for example ...
		// END   Private data of an RouteSetQuadCrossover object
		float pSwap;
};

#endif	/* _ROUTESETQUADCROSSOVER_H */

