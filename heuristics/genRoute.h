#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

#define mem(list, val) memset(list, (val), sizeof(list))
#define pb push_back

typedef vector<int> _Route;
typedef vector<vector<int> > _RouteSet;
typedef vector<vector<double> > Matrix;

#define MAX 200
#define INF 1e8

enum status {NEW_ROUTE_ADDED, SIZE_OVERFLOW, NO_PATH};

int minRouteSize, maxRouteSize;

vector<double> popularity;

struct node{
    int id;
    double dist;

    node(int i, double w){
        id = i;
        dist = w;
    }

    friend bool operator<(const node& l, const node& r)
    {
        return l.dist > r.dist; // since default pq is max pd
    }
};

void readData(string filename, Matrix &matrix)
{
	FILE *fp = fopen(filename.c_str(), "r");
	
    int siz;
	double num;
    fscanf(fp, "%d", &siz);

	vector<double> temp;
    for(int i=0; i<siz; i++){
		for(int j=0; j<siz; j++){
			fscanf(fp, "%lf", &num);
			temp.push_back(num);
		}
		matrix.push_back(temp);
		temp.clear(); 
	}

    fclose(fp);
}

template<class T>
void print_matrix(vector<vector<T> > &m)
{
    int d = m.size();

    for(int i = 0; i < d; i++) {
        int l = m[i].size();
        for(int j = 0; j < l; j++) {
            printf("%f ", m[i][j]);
        }
        printf("\n");
    }
}

pair<int, int> getHighestDemandPair(Matrix &demand)
{
    int from, to;
    double maxDemand = 0;

    for(int i=0; i<demand.size(); i++){
        for(int j=0; j<demand[i].size(); j++){
            if(demand[i][j] > maxDemand){
                from = i, to = j, maxDemand = demand[i][j];
            }
        }
    }
    
    //cout << "check: " << from << ' ' << to << ' ' << maxDemand << endl;
    return make_pair(from, to);
}

int highestDemandDestination(int from, Matrix &demand)
{
	int ret;
	double maxDemand = -INF;
	for(int i=0; i<demand[from].size(); i++){
		if(demand[from][i] > maxDemand){
			maxDemand = demand[from][i];
			ret = i;
		}
	}
	return ret;
}

double normalize(Matrix &mat)
{
    double mx = 0;
    for(int i=0; i<mat.size(); i++){
        auto it = max_element(begin(mat[i]), end(mat[i]));
        mx = max(mx, *it);
    }

    if(mx == 0) return 0;
    for(int i=0; i<mat.size(); i++){
        for(int j=0; j<mat[i].size(); j++){
            if(mat[i][j] == -1 || mat[i][j] == 0 || mat[i][j] >= INF) mat[i][j] = INF; ///for no edge case
            else mat[i][j] /= mx;
        }
    }

    return mx;
}

status getBestRoute(int from, int to, Matrix &distance, Matrix &demand, _Route &ret, double dw)
{
	//cout << "from: " << from << " to: " << to << " demand: " << demand[from][to] << endl; 

    Matrix tempDist = distance, tempDemand = demand;

    normalize(tempDemand); ///normalize demands
    normalize(tempDist); ///normalize distance
        
    ///transform to fit shortest path problem
    for(int i=0; i <tempDist.size(); i++) {
        for(int j=0; j<tempDist[i].size(); j++) {
            if(tempDist[i][j] <= 1.0) tempDist[i][j] = (1 - dw) * tempDist[i][j] +  dw * 0.5 * (1/(tempDemand[i][to]+1) + 1/(tempDemand[j][to]+1));
        }
    }
    
    int numberOfNodes = distance.size();
    double djDist[numberOfNodes+1], parent[numberOfNodes+1];

    for(int i=0; i<numberOfNodes; i++){
        djDist[i] = INF;
        parent[i] = -1;
    }

    djDist[from] = 0; ///path cost = distance + 1/(demand + 1)

    priority_queue<node> Q;
    Q.push(node(from, djDist[from]));

    bool dequed[numberOfNodes+1];
    mem(dequed, false);

    while(!Q.empty()){

	    int fr = Q.top().id;
	    //cout << fr <<endl;

	    if(fr == to) break;

	    if(dequed[fr]){
		    Q.pop();
		    continue;
	    }

	    djDist[fr] = Q.top().dist;
	    dequed[fr] = true;
	    Q.pop();

	    for(int i=0; i<tempDist[fr].size(); i++){
		    if(tempDist[fr][i] > 1.0 || fr == i) continue;

		    if(djDist[i] > djDist[fr] + tempDist[fr][i]){
			    djDist[i] = djDist[fr] + tempDist[fr][i];
			    parent[i] = fr;

			    Q.push(node(i, djDist[i]));
		    }
	    }
    }

    if(parent[to] == -1){ //no path
        //cout << "no path\n";
        return NO_PATH;
    }

    _Route temp;

	int cur = to;
	temp.push_back(cur);
	while(cur != from){
		cur = parent[cur];
		temp.push_back(cur);
	}

    //for(int i=temp.size()-1; i>=0; i--) cout << temp[i] << ' ' ;
    //cout << endl;

    if(ret.size() + temp.size() -1 <= maxRouteSize){
        assert(temp.size() != maxRouteSize + 1);
        if(ret.size()) ret.pop_back();
        ret.insert(ret.end(), temp.rbegin(), temp.rend());
        return NEW_ROUTE_ADDED;
    }
    return SIZE_OVERFLOW;
	
}

bool recur(Matrix &distance, Matrix &demand, Matrix &choice, _Route &route, double dw, int from, int to = -1) ///generates a single route
{	
	//getchar();
	
	if(choice[from][to] <= 0 || demand[from][to] <= 0){
		//cout << "No demand left\n";
		return (route.size() >= minRouteSize && route.size() <= maxRouteSize); ///no more demand left
	}
	
	status ret = getBestRoute(from, to, distance, demand, route, dw); ///add a route (part)
	
	if(ret == SIZE_OVERFLOW){
		//cout << "size overflow\n";
		return (route.size() >= minRouteSize && route.size() <= maxRouteSize);
	}
	
	if(ret == NO_PATH){
		//cout << "no path between " << from << ' ' << to << " demand: " <<  choice[from][to] << endl;
		choice[from][to] = 0;
		choice[to][from] = 0;
		to = highestDemandDestination(from, choice);
		
		return recur(distance, demand, choice, route, dw, from, to);
	}
	
	//segment added to route
	//cout << "route segment added\ncurrent size: " << route.size() << endl;
	
	for(int i=0; i<route.size(); i++){
		for(int j=0; j<route.size(); j++){
			demand[route[i]][route[j]] = 0;
			choice[route[i]][route[j]] = 0;
		}
	}
	
	for(int i=0; i<route.size(); i++){
		int ind = route[i];
		for(int j=0; j<distance[ind].size(); j++) distance[j][ind] = -1;
	}
	
	from = to;
	to = highestDemandDestination(from, choice);
	return recur(distance, demand, choice, route, dw, from, to);
}

bool checkRoute(const _Route &route, const Matrix &distance, int maxRouteSize, int minRouteSize)
{
	/*
    if(route.size() > maxRouteSize)
        return false;
    if(route.size() < minRouteSize)
        return false;
    */
        
    assert(route.size() >= minRouteSize && route.size() <= maxRouteSize);

    int n = distance.size();
    vector<bool> isInRoute(n, false);

    for(int i = 0; i < route.size(); i++) {
        /*
        if(isInRoute[route[i]]) {
            return false;
        }
        */
        assert(!isInRoute[route[i]]);
        isInRoute[route[i]] = true;
    }

    for(int i = 0; i < route.size() - 1; i++) {
        /*
        if(distance[route[i]][route[i + 1]] == -1) {
            return false;
        }
        */
        assert(distance[route[i]][route[i + 1]] != -1); 
        assert(distance[route[i]][route[i + 1]] != INF); 
        assert(distance[route[i]][route[i + 1]] != INFINITY); 
        assert(distance[route[i]][route[i + 1]] < 1e9); 
    }
    return true;
}

void getBestRouteSet(Matrix &dist, Matrix &demand, int numberOfRoutes, _RouteSet &RS, double dw)
{
    //puts("Inside genBestRouteSet");
    Matrix tempDist, tempDemand = demand, choice;

    for(int i=0; i<numberOfRoutes; i++){
        tempDist = dist;
        choice = tempDemand;
        
        pair<int, int> pp = getHighestDemandPair(choice);
		int from = pp.first, to = pp.second;

        //if(popularity[from] > popularity[to]) swap(from, to);
        
        _Route route;
        bool added = recur(tempDist, tempDemand, choice, route, dw, from, to);
        
        if(!added) i--;
        else{
			//cout << "route added\n";
			RS.push_back(route);
		}
        
    }
    
    for(auto route : RS)
    {
	assert(checkRoute(route, dist, maxRouteSize, minRouteSize));
    } 
    ///getHighestDemandPair(tempDemand);
}


