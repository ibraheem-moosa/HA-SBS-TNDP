/* 
 * File:   popInit.h
 * Author: MAN
 *
 * Created on September 7, 2013, 7:01 AM
 */

#ifndef _POPINIT_H
#define	_POPINIT_H

#include <eo>
#include "routeSet.h"
#include "inputData.h"

void popInit(eoPop< RouteSet<double> >& _pop, int popSize, int routeSetSize, eoEvalFunc< RouteSet<double> >& _eval)
{
    RouteSet<double> initRs;
    vector< Route<double> > initV;
    ifstream fin(INITIAL_ROUTE_SET);
    for (int iroute = 0; iroute < routeSetSize; iroute++)
    {
        Route<double> initR;
        vector<int> initNodeList(VERTICES_NO, 0);

        list<int> initL;
        int node;
        fin >> node;
        while (node != -1)
        {
            initL.push_back(node);
            initNodeList[node] = 1;
            fin >> node;
        }
        
        //cout << initL.front() << " " << initL.back() << " " << DS[initL.front()][initL.back()] << endl;
	//cout << initL.size() << endl;

        if(initL.size() > parameters["maxRouteLength"]){
            int del = initL.size() - parameters["maxRouteLength"];
            if(rng.uniform() > 0.5){
                for(int i=0; i<del; i++) initL.pop_front();
            }
            else{
               int newSize = initL.size() - del;
               initL.resize(newSize); 
            }
        } 
	//cout << initL.size() << endl;
        
        initR.setR(initL);
        initR.setNodeList(initNodeList);
        cout << initL.front() << " " << initL.back() << " " << DS[initL.front()][initL.back()] << endl;
	initR.fitness(1.0 / DS[initL.front()][initL.back()]); //fitness 1/ds
        initV.push_back(initR);
    }


    initRs.setRs(initV);
    _eval(initRs);

    //std::cout<<initRs<<std::endl; //comment after test

    for (int igeno = 0; igeno < popSize; igeno++)
    {
        _pop.push_back(initRs);
    }
}

#endif	/* _POPINIT_H */


