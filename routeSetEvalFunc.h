/* 
 * File:   routeSetEvalFunc.h
 * Author: MAN
 *
 * Created on September 7, 2013, 6:44 PM
 */

#ifndef _ROUTESETEVALFUNC_H
#define	_ROUTESETEVALFUNC_H

#include <queue>

#include "routeSet.h"
//#include "eoEvalFunc.h"
//#include "inputData.h"


struct vertex
{
    int id, d, p, t;
    vertex(int _id, int _d, int _p, int _t) : id(_id), d(_d), p(_p), t(_t)
    {
    }
    friend bool operator<(const vertex &l, const vertex &r)
    {
        return l.d > r.d;
    }
};


struct djikstra_edge
{
    int to;
    int weight;
    bool isTransfer;
    djikstra_edge(int _to, int _weight, bool _isTransfer) : to(_to), weight(_weight), isTransfer(_isTransfer)
    {
    }
};


#pragma GCC push_options
#pragma GCC optimize("O3")
void allPairShortestPath(vector < vector<djikstra_edge> > & eAdjList, vector <vector<int> > & dist, vector < vector <int> > & transfer)
{
    int V = eAdjList.size();
    for(int i = 0; i < V; i++)
    {
        //find shortest path from i to all other vertices
        vector<bool> visited(V, false);
        vector<vertex> vertices(V, vertex(0, 1000000, -1, 0));
        for(int j = 0; j < V; j++){
            vertices[j].id = j;
        }
        vertices[i].d = 0;
        priority_queue<vertex> Q;
        Q.push(vertices[i]);
        while(Q.size() != 0)
        {
            vertex u = Q.top();
            Q.pop();
            if(visited[u.id] == true) continue;
            visited[u.id] = true;
            for(djikstra_edge e : eAdjList[u.id])
            {
		if(vertices[u.id].d + e.weight < 0)
			printf("%d %d\n", vertices[u.id].d, e.weight);
		assert(vertices[u.id].d + e.weight >= 0);
                if(visited[e.to]) continue;
                if(vertices[e.to].d > vertices[u.id].d + e.weight)
                {
                    vertices[e.to].d = vertices[u.id].d + e.weight;
                    vertices[e.to].p = u.id;
                    if(e.isTransfer)
                        vertices[e.to].t = vertices[u.id].t + 1;
                    else
                        vertices[e.to].t = vertices[u.id].t;
                    Q.push(vertices[e.to]);
                }
            }
        }
        for(int j = 0; j < V; j++){
            dist[i][j] = vertices[j].d;
            transfer[i][j] = vertices[j].t;
	    assert(dist[i][j] >= 0);
        }
    }
}
#pragma GCC pop_options


#pragma GCC push_options
#pragma GCC optimize("O3")
void floydWarshall(vector< vector<int> > & sDist, int vertexNo, vector< vector<int> > & transfer)
{
    for (int k = 0; k < vertexNo; k++)
    {
        for (int i = 0; i < vertexNo; i++)
        {
            int sDist_i_k = sDist[i][k];
            if(sDist_i_k == INFINITY)
                continue;
            for (int j = i + 1; j < vertexNo; j++)
            {
                if (sDist[k][j] == INFINITY)
                {
                    continue;
                }
                if ((sDist_i_k + sDist[k][j]) < sDist[i][j])
                {
                    sDist[j][i] = sDist[i][j] = sDist_i_k + sDist[k][j];
                    transfer[i][j] = transfer[j][i] = transfer[i][k] + transfer[k][j];
                }
            }
        }
    }
}
#pragma GCC pop_options

template <class EOT>
class RouteSetEvalFunc : public eoEvalFunc<EOT>
{
public:
    /// Ctor - no requirement
    // START eventually add or modify the anyVariable argument

    RouteSetEvalFunc()
    {
        // START Code of Ctor of an RouteSetEvalFunc object
        // END   Code of Ctor of an RouteSetEvalFunc object
    }

    /** Actually compute the fitness
     *
     * @param EOT & _eo the EO object to evaluate
     *                  it should stay templatized to be usable
     *                  with any fitness type
     */
    void operator()(EOT & _eo)
    {
        // test for invalid to avoid recomputing fitness of unmodified individuals
        if (_eo.invalid())
        {
            // START Code of computation of fitness of the RouteSet object
            vector< Route<double> > & routeSet = _eo.mutableRs();

            int dist[VERTICES_NO][VERTICES_NO];
            int transfer[VERTICES_NO][VERTICES_NO];
            list<int> O2E[VERTICES_NO];
            vector<int> E2O;
            vector<int> routeNo;
            vector< vector<int> > newMap(VERTICES_NO, vector<int> (routeSet.size(), 0));
            //int total_route_length = 0;
            for (int r = 0; r < routeSet.size(); r++)
            {
                //total_route_length += routeSet[r].size();
                list<int>::const_iterator it = routeSet[r].R().begin();
                for (; it != routeSet[r].R().end(); it++)
                {
                    O2E[*it].push_back(E2O.size());
                    newMap[*it][r] = E2O.size();
                    E2O.push_back(*it);
                    routeNo.push_back(r);
                }
            }
//            printf("total route length %d new vertex count %d\n", total_route_length, E2O.size()); 
            const int newVertexCount = E2O.size();
            vector< vector<int> > eDist(newVertexCount, vector<int>(newVertexCount, 0));
            vector< vector<int> > eTransfer(newVertexCount, vector<int>(newVertexCount, 0));
            vector< vector<djikstra_edge> > eDistAdjList(newVertexCount);
            for (int i = 0; i < newVertexCount; i++)
            {
                eDist[i][i] = 0;
                for (int j = i + 1; j < newVertexCount; j++)
                {
                    int I = E2O[i], J = E2O[j];
                    if (I == J)
                    {
                        eDist[i][j] = eDist[j][i] = TRANSFER_PENALTY;
                        eDistAdjList[i].push_back(djikstra_edge(j, TRANSFER_PENALTY, true));
                        eDistAdjList[j].push_back(djikstra_edge(i, TRANSFER_PENALTY, true));
                        eTransfer[i][j] = 1;
                    }
                    else
                    {
                        eDist[i][j] = eDist[j][i] = INFINITY;
                    }
                }
            }
            list<int>::const_iterator next, it; 
            for (int r = 0; r < routeSet.size(); r++)
            {
                next = routeSet[r].R().begin();
                it = next++;
                for (; next != routeSet[r].R().end(); it++, next++)
                {
                    int i = *it;
                    int j = *next;
                    int I = newMap[i][r];
                    int J = newMap[j][r];
                    eDist[J][I] = eDist[I][J] = tr[i][j];
		    //assert(tr[i][j] != INFINITY);
		    if(tr[i][j] != INFINITY)
		    {
                    	eDistAdjList[J].push_back(djikstra_edge(I, tr[i][j], false));
                    	eDistAdjList[I].push_back(djikstra_edge(J, tr[i][j], false));
		    }
                }
            }
            /*
            int edgeCountInNewGraph = 0;
            for(int i = 0; i < newVertexCount; i++) {
                for(int j = 0; j < newVertexCount; j++) {
                    if(eDist[i][j] != INFINITY)
                        edgeCountInNewGraph++;
                }
            }
            printf("In New Graph V: %d E: %d\n", newVertexCount, edgeCountInNewGraph);
            */
            //auto eDistCopy(eDist);
            //auto eTransferCopy(eTransfer);
            floydWarshall(eDist, newVertexCount, eTransfer);
	    //allPairShortestPath(eDistAdjList, eDist, eTransfer);
            //allPairShortestPath(eDistAdjList, eDistCopy, eTransferCopy);
            //for(int i = 0; i < newVertexCount; i++){
            //    for(int j = 0; j < newVertexCount; j++){
            //        assert(eDistCopy[i][j] == eDist[i][j]);
            //        printf("%d ", eTransfer[i][j]);
            //    }
            //    printf("\n");
            //}
            //getchar();
            for (int i = 0; i < VERTICES_NO; i++)
            {
                dist[i][i] = 0;
                for (int j = i + 1; j < VERTICES_NO; j++)
                {
                    int min = INFINITY, minTransfer = INFINITY;

                    list<int>::iterator iIt = O2E[i].begin();
                    for (; iIt != O2E[i].end(); iIt++)
                    {
                        list<int>::iterator jIt = O2E[j].begin();
                        for (; jIt != O2E[j].end(); jIt++)
                        {
                            if (eDist[*iIt][*jIt] < min)
                            {
                                min = eDist[*iIt][*jIt];
                                minTransfer = eTransfer[*iIt][*jIt];
                            }
                        }
                    }

                    dist[i][j] = dist[j][i] = min;
                    transfer[i][j] = transfer[j][i] = minTransfer;
                }
            }

            double D[] = {0, 0, 0}, Dun = 0, ATT = 0, totalDemand = 0, totalSatisfied = 0, T = 0;

            for (int i = 0; i < VERTICES_NO; i++)
            {
                for (int j = i + 1; j < VERTICES_NO; j++)
                {
                    totalDemand += d[i][j];
                    if (dist[i][j] == INFINITY)
                    {
                        Dun += d[i][j];
                    }
                    else if (transfer[i][j] > 2)
                    {
                        Dun += d[i][j];
                        //ATT += d[i][j] * dist[i][j];
                        //totalSatisfied += d[i][j];
                        //  cout << "more than 2";
                    }
                    else
                    {
                        D[transfer[i][j]] += d[i][j];
                        ATT += d[i][j] * dist[i][j];
                        T += d[i][j] * dist[i][j];
                        totalSatisfied += d[i][j];
                    }
                }
            }

            T += Dun * (parameters["OptimalATT"] + 50);
            //_eo.fitness(-T); //minimize
            _eo.actualFitness = -T;
            _eo.D[0] = D[0] / totalDemand;
            _eo.D[1] = D[1] / totalDemand;
            _eo.D[2] = D[2] / totalDemand;
            _eo.Dun = Dun / totalDemand;
            _eo.ATT = ATT / totalSatisfied;

        }
    }
private:
    // START Private data of an RouteSetEvalFunc object
    //  varType anyVariable;		   // for example ...
    // END   Private data of an RouteSetEvalFunc object
};

#endif	/* _ROUTESETEVALFUNC_H */

