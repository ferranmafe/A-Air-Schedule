#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <climits>
#include <utility>
#include <fstream>
#include <sstream>
#include <stdlib.h>
using namespace std;

typedef vector<vector<int> > Graph;
typedef vector<int> flight;
// /Users/danielmartinezbordes/ClionProjects/A-Air-Sch/test_entry.txt


void read_new_input(vector<flight>& flights_info_vector) {
  //Generem una matriu buida, que es la que retornarem plena amb els vols a tractar

  //Rebem el path del fitxer a obrir i tractar
  cout << "Siusplau, introdueixi el path del fitxer de vols del que vol generar el schedule:" << endl;
  string flight_file_path;
  cin >> flight_file_path;

  //Obrim el fitxer
  fstream flight_file;
  flight_file.open( flight_file_path.c_str(), ios::in );

  //Si podem obrir el fitxer, llegim el mateix linia per linia. Sino, retornem un
  //error i sortim de l'aplicació
  if( flight_file.is_open() ) {
    string s;
    while(getline( flight_file, s )) {
      //Per cada linia, la descomposem en strings separats per " ", a efectes de
      //fer la transformació a int i guardarnos el resultat
      flight new_flight;
      istringstream line(s);
      for (string str; line >> str;) new_flight.push_back(atoi(str.c_str()));
      flights_info_vector.push_back(new_flight);
    }
    flight_file.close();
  }
  else {
    cout << "Error obrint el fitxer." << endl;
    exit(1);
  }
}

/* PROCEDURE:
 * 1_Generem el graf buit, de tamany 2 * flight_input.size() + 2
 * 2_Recorrem el graf, afegim el ID de cada vol a cada vertex del graf i afegim
 * la connexió dels vertex del grup B a t
 * 3_Afegim les connexions de s als vertex del grup A
 * 4_Per cada vertex del grup A hem de mirar a quins vertex del grup B es pot connectar
 * (Vol destí A = Vol origen B i temps arribada A + 15 <= temps sortida B)
 * 5_Connectem els vertex sueltos a s i t (Opcional, no se si cal fer-ho)
 *
 * NOTE: Definim els id dels nodes S (-1) i T (-2)
*/
void parse_last_input_to_max_flow_adjacence_list_version_1(const vector<flight>& flight_input, Graph& adjacence_matrix, Graph& capacity_matrix) {
  //Recorrem el graf de sortida. Fem les connexions amb t
  for (int i = flight_input.size(); i < 2 * flight_input.size(); ++i) {
    //En el cas dels vertex del grup B (els que arriben a t) a més de l'ID del vol
    //afegim la aresta que arriba a t
      adjacence_matrix[i][2 * flight_input.size() + 1] = 1;
      capacity_matrix[i][2 * flight_input.size() + 1] = 1;
  }

  //Connectem s a tots els vertex del grup A
  for (int i = 0; i < flight_input.size(); ++i) {
    adjacence_matrix[2 * flight_input.size()][i] = 1;
    capacity_matrix[2 * flight_input.size()][i] = 1;
  }

  //Recorrem tots els elements del grup A i per cada vol fem la connexió entre grup
  //A i grup B si (Vol destí A = Vol origen B i temps arribada A + 15 <= temps sortida B)
  for (int i = 0; i < flight_input.size(); ++i) {
    for (int j = 0; j < flight_input.size(); ++j) {
      if (flight_input[i][1] == flight_input[j][0] && flight_input[i][3] + 15 <= flight_input[j][2]) {
        adjacence_matrix[i][flight_input.size() + j] = 1;
        capacity_matrix[i][flight_input.size() + j] = 1;
      }
    }
  }
}


void make_connections_ith_flight(const vector<flight>& flight_input, Graph& adjacence_matrix, Graph& capacity_matrix, const int& i, const int& j) {
  int k = 0;
  while(k < flight_input.size()) {
    if (flight_input[j][1] == flight_input[k][0] && flight_input[j][3] + 15 < flight_input[k][2]) {
      adjacence_matrix[i][flight_input.size() + k] = 1;
      capacity_matrix[i][flight_input.size() + k] = 1;
      make_connections_ith_flight(flight_input, adjacence_matrix, capacity_matrix, i, k);
    }
    ++k;
  }
}

void parse_last_input_to_max_flow_adjacence_list_version_2(const vector<flight>& flight_input, Graph& adjacence_matrix, Graph& capacity_matrix) {
  //Recorrem el graf de sortida. Fem les connexions amb t
  for (int i = flight_input.size(); i < 2 * flight_input.size(); ++i) {
    //En el cas dels vertex del grup B (els que arriben a t) a més de l'ID del vol
    //afegim la aresta que arriba a t
      adjacence_matrix[i][2 * flight_input.size() + 1] = 1;
      capacity_matrix[i][2 * flight_input.size() + 1] = 1;
  }

  //Connectem s a tots els vertex del grup A
  for (int i = 0; i < flight_input.size(); ++i) {
    adjacence_matrix[2 * flight_input.size()][i] = 1;
    capacity_matrix[2 * flight_input.size()][i] = 1;
  }
  for (int i = 0; i < flight_input.size(); ++i) {
    make_connections_ith_flight(flight_input, adjacence_matrix, capacity_matrix, i, i);
  }
}

int BreadthFirstSearch(const Graph& E, const Graph& C, Graph& F, vector<int>& P, int s, int t){
    int n = C.size();
    vector<int> M = vector<int>(n);
    M[s] = INT_MAX;
    std::queue<int> Q;
    Q.push(s);
    while (!Q.empty()){
        int u = Q.front();
        Q.pop();
        for (int v = 0; v < E[u].size(); v++){
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

int edmondsKarp(const Graph& E, const Graph& C, Graph& F, int s, int t){
    int f = 0;
    int n = C.size();
    while (true) {
        vector<int> P(n, -1);
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


void generate_schedule_i(const Graph& graph, const int& i, vector<int>& schedule_i) {
  int j = 0;
  bool connection_found = false;
  while(j < ((graph.size() - 2) / 2) && !connection_found) {
    if (graph[i][j] == 1) {
      connection_found = true;
      cout << "Adding element " << j + 1 << endl;
      schedule_i.push_back(j + 1);
      generate_schedule_i(graph, j, schedule_i);
    }
    ++j;
  }
}
/* PROCEDURE:
 * 1_Recorrem les connexions de cada node del grup B amb t. Aquells nodes que no tinguin fluxe seran
 * els que inicien els schedules, perque no van precedits de cap altre.
 * 2_Cada cop que trobem un dels nodes inici, el recorrem fins que el node final ja no tingui connexions.
*/
void generate_output(const Graph& graph) {
  vector<vector<int> > schedule;
  //Recorrem tots els nodes de B. Per cada node que no tingui connexió amb t, busquem el schedule
  for (int i = ((graph.size() - 2) / 2); i < graph.size() - 2; ++i) {
    if (graph[i][graph.size() - 1] != 1) {
      vector<int> schedule_i;
      cout << "Adding element " << (i%((graph.size() - 2) / 2)) + 1 << endl;
      schedule_i.push_back((i%((graph.size() - 2) / 2)) + 1);
      generate_schedule_i(graph, i, schedule_i);
      schedule.push_back(schedule_i);
    }
  }
  cout << schedule.size() << endl;
  for (int i = 0; i < schedule.size(); ++i) {
    for(int j = 0; j < schedule[i].size(); ++j) {
      cout << schedule[i][j];
      if (j != schedule[i].size() - 1) cout << " ";
    }
    cout << endl;
  }
}

void print_flow_graph(Graph& graph) {
  cout << "Graph Size: " << graph.size() << endl;
  for (int i = 0; i < graph.size(); ++i) {
    for(int j = 0; j < graph.size(); ++j) {
      cout << graph[i][j] << " ";
    }
    cout << endl;
  }
}

int main() {
    cout << "Benvingut al programa " << '"' << "Air Schedule v1.0" << '"' << "." << endl;

  //Demanem la versió del programa. Si aquesta es diferent de 1 o 2, la tornem a demanar
  cout << "Siusplau, introdueixi la versió del programa a utilitzar (1 - Versió 1, 2 - Versió 2):" << endl;
  int v;
  cin >> v;
  while (v != 1 and v != 2) {
    cout << "La versió introduida no es correcta. Siusplau, introdueixi la versió sobre la que vol executar el programa (1 - Versió 1, 2 - Versió 2):" << endl;
    cin >> v;
  }

  //Llegim la entrada d'un fitxer passat com a paràmetre
  vector<flight> flight_input;
  read_new_input(flight_input);

  //Convertim la entrada en un graf amb pesos sobre el que aplicar max flow (que serà diferent en funció de si acceptem que els pilots
  //viatjin o no)
  Graph adjacence_matrix(2 * flight_input.size() + 2, vector<int>(2 * flight_input.size() + 2, 0));
  Graph capacity_matrix (2 * flight_input.size() + 2, vector<int>(2 * flight_input.size() + 2, 0));
  Graph flow_matrix     (2 * flight_input.size() + 2, vector<int>(2 * flight_input.size() + 2, 0));
  if (v == 1) parse_last_input_to_max_flow_adjacence_list_version_1(flight_input, adjacence_matrix, capacity_matrix);
  else parse_last_input_to_max_flow_adjacence_list_version_2(flight_input, adjacence_matrix, capacity_matrix);

  //print_flow_graph(adjacence_matrix);
  //print_flow_graph(capacity_matrix);
  int f = edmondsKarp(adjacence_matrix, capacity_matrix, flow_matrix, 2* flight_input.size(), 2* flight_input.size() + 1);
    cout << "Max Flow: " << f << endl;
    print_flow_graph(flow_matrix);
    generate_output(flow_matrix);
}
