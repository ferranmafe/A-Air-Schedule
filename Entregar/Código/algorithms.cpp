#include <vector>
#include <queue>
#include <stack>
#include <cmath>
using namespace std;

typedef vector<vector<int> > Graph;

int BreadthFirstSearch(const Graph& C, Graph& F, vector<int>& P, int s, int t){
    int n = C.size();
    vector<int> M = vector<int>(n);
    M[s] = INT_MAX;
    queue<int> Q;
    Q.push(s);
    while (!Q.empty()){
        int u = Q.front();
        Q.pop();
        for (int v = 0; v < n; v++){
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
    return 0;
}

int EdmondsKarp(const Graph& C, Graph& F, int s, int t){
    int f = 0;
    int n = C.size();
    while (true) {
        vector<int> P(n, -1);
        P[s] = s;
        int m = BreadthFirstSearch(C,F,P,s,t);
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

bool DinicBFS(const Graph& C, const Graph& F, vector<int>& L, int s, int t){
    int n = C.size();
    queue<int> Q;
    Q.push(s);
    while (!Q.empty()){
        int u = Q.front();
        Q.pop();
        for (int v = 0; v < n; v++){
            if (C[u][v] - F[u][v] > 0 && L[v] == -1) {
                L[v] = L[u] + 1;
                Q.push(v);
            }
        }
    }
    return L[t] >= 0;
}

int sendFlow(const Graph& C, Graph& F, const vector<int>& L, int u, int t, int flow){
    if (u == t){
        return flow;
    }
    int n = C.size();
    for (int v = 0; v < n; v++){
        if ((L[v] == L[u] + 1) && (C[u][v] - F[u][v] > 0)) {
            int currentFlow = fmin(flow, C[u][v] - F[u][v]);
            int bottleneckFlow = sendFlow(C,F,L,v,t,currentFlow);
            if (bottleneckFlow > 0){
                F[u][v] += bottleneckFlow;
                F[v][u] -= bottleneckFlow;
                return bottleneckFlow;
            }
        }
    }
    return 0;
}

int DinicAlgorithm(const Graph& C, Graph& F, int s, int t){
    int f = 0;
    int n = C.size();
    vector<int> L(n,-1);
    L[s] = 0;
    while(DinicBFS(C,F,L,s,t)){
        while (int sentFlow = sendFlow(C,F,L,s,t,INT_MAX) > 0)
            f += sentFlow;
        L = vector<int>(n,-1);
        L[s] = 0;
    }
    return f;
}
