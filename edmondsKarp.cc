#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <cmath>
#include <climits>
using namespace std;

typedef vector<vector<int> > Graph;

int BreadthFirstSearch(const Graph& E, const Graph& C, Graph& F, vector<int>& P, int s, int t){
    int n = C.size();
    vector<int> M = vector<int>(n);
    M[s] = INT_MAX;
    queue<int> Q;
    Q.push(s);
    while (!Q.empty()){
        int u = Q.front();
        Q.pop();
        for (int v = 0; v < E[u].size(); v++){
            if (E[u][v] == 1){
                if (C[u][v] - F[u][v] > 0 && P[v] == -1) {
                    P[v] = u;
                    M[v] = fmin(M[u], C[u][v] - F[u][v]);
                    if (v != t){
                        Q.push(v);
                    }
                    else {
                        return M[t];
                    }
                }
            }
        }
    }
    return 0;
}

int edmondsKarp(const Graph& E, const Graph& C, Graph& F, int s, int t){
    int f = 0;
    int n = C.size();
    while (true) {
        vector<int> P = vector<int>(n, -1);
        P[s] = s;
        int m = BreadthFirstSearch(E,C,F,P,s,t);
        if (m == 0){
            return f;
        }
        f += m;
        int v = t;
        while (P[v] != v) {
            int u = P[v];
            F[u][v] += m;
            F[v][u] -= m;
            v = u;
        }
    }
}

int main(){
    cout << INT_MAX << endl;
}