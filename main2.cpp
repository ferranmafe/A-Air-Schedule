#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "algorithms.cpp"

using namespace std;

typedef vector<int> flight;
// /Users/danielmartinezbordes/ClionProjects/A-Air-Sch/test_entry.txt


void read_new_input(vector<flight> &flights_info_vector) {
    //Generem una matriu buida, que es la que retornarem plena amb els vols a tractar

    //Rebem el path del fitxer a obrir i tractar
    cout << "Siusplau, introdueixi el path del fitxer de vols del que vol generar el schedule:" << endl;
    string flight_file_path;
    cin >> flight_file_path;

    //Obrim el fitxer
    fstream flight_file;
    flight_file.open(flight_file_path.c_str(), ios::in);

    //Si podem obrir el fitxer, llegim el mateix linia per linia. Sino, retornem un
    //error i sortim de l'aplicació
    if (flight_file.is_open()) {
        string s;
        while (getline(flight_file, s)) {
            //Per cada linia, la descomposem en strings separats per " ", a efectes de
            //fer la transformació a int i guardarnos el resultat
            flight new_flight;
            istringstream line(s);
            for (string str; line >> str;) new_flight.push_back(atoi(str.c_str()));
            flights_info_vector.push_back(new_flight);
        }
        flight_file.close();
    } else {
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
void parse_last_input_to_max_flow_adjacence_list_version_1(const vector<flight> &flight_input, Graph &capacity_matrix) {
    unsigned long n = flight_input.size();

    for (int i = 0; i < n; ++i) {
        capacity_matrix[2 * n][i] = 1;
        capacity_matrix[i][2 * n + 3] = 1;
    }

    for (int i = n; i < 2 * n; ++i) {
        capacity_matrix[2 * n + 2][i] = 1;
        capacity_matrix[i][2 * n + 1] = 1;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (flight_input[i][1] == flight_input[j][0] && flight_input[i][3] + 15 <= flight_input[j][2]) {
                capacity_matrix[n + i][j] = 1;
            }
        }
    }
}

/*
void make_connections_ith_flight(const vector<flight>& flight_input, Graph& capacity_matrix, const int& i, const int& j) {
  int k = 0;
  while(k < flight_input.size()) {
    if (flight_input[j][1] == flight_input[k][0] && flight_input[j][3] + 15 < flight_input[k][2]) {
      capacity_matrix[i][flight_input.size() + k] = 1;
      make_connections_ith_flight(flight_input, capacity_matrix, i, k);
    }
    ++k;
  }
}

void parse_last_input_to_max_flow_adjacence_list_version_2(const vector<flight>& flight_input, Graph& capacity_matrix) {
  //Recorrem el graf de sortida. Fem les connexions amb t
  for (int i = flight_input.size(); i < 2 * flight_input.size(); ++i) {
    //En el cas dels vertex del grup B (els que arriben a t) a més de l'ID del vol
    //afegim la aresta que arriba a t
      capacity_matrix[i][2 * flight_input.size() + 1] = 1;
  }

  //Connectem s a tots els vertex del grup A
  for (int i = 0; i < flight_input.size(); ++i) {
    capacity_matrix[2 * flight_input.size()][i] = 1;
  }
  for (int i = 0; i < flight_input.size(); ++i) {
    make_connections_ith_flight(flight_input, capacity_matrix, i, i);
  }
}
*/

void parse_last_input_to_max_flow_adjacence_list_version_2(const vector<flight> &flight_input, Graph &capacity_matrix) {
    //Recorrem el graf de sortida. Fem les connexions amb t
    vector<bool> visited_vertex(flight_input.size(), false);
    for (int i = flight_input.size(); i < 2 * flight_input.size(); ++i) {
        //En el cas dels vertex del grup B (els que arriben a t) a més de l'ID del vol
        //afegim la aresta que arriba a t
        capacity_matrix[i][2 * flight_input.size() + 1] = 1;
    }

    //Connectem s a tots els vertex del grup A
    for (int i = 0; i < flight_input.size(); ++i) {
        capacity_matrix[2 * flight_input.size()][i] = 1;
    }
    stack<int> S;
    for (int i = 0; i < flight_input.size(); ++i) {
        S.push(i);
        while (!S.empty()) {
            int u = S.top();
            S.pop();
            for (int j = 0; j < flight_input.size(); ++j) {
                if (visited_vertex[j]) {
                    if (flight_input[u][1] == flight_input[j][0] && flight_input[u][3] + 15 <= flight_input[j][2]) {
                        capacity_matrix[u][flight_input.size() + j] = 1;
                        for (int k = 0; k < flight_input.size(); ++k) {
                            if (capacity_matrix[j][flight_input.size() + k]) {
                                capacity_matrix[u][flight_input.size() + k] = 1;
                                capacity_matrix[i][flight_input.size() + k] = 1;
                            }
                        }
                    }
                } else {
                    if (flight_input[u][1] == flight_input[j][0] && flight_input[u][3] + 15 < flight_input[j][2]) {
                        capacity_matrix[u][flight_input.size() + j] = 1;
                        capacity_matrix[i][flight_input.size() + j] = 1;
                        S.push(j);
                    }
                }
            }
            visited_vertex[u] = true;
        }
    }
}


void generate_schedule_i(const Graph &graph, int i, vector<int> &schedule_i) {
    int n = (graph.size() - 4) / 2;

    bool last_flight = false;
    while (not last_flight) {
        last_flight = true;
        for (int j = 0; j < n; ++j) {
            if (graph[i][j] == 1) {
                schedule_i.push_back(j);
                last_flight = false;
                i = j+n;
            }
        }
    }
}

/* PROCEDURE:
 * 1_Recorrem les connexions de cada node del grup B amb t. Aquells nodes que no tinguin fluxe seran
 * els que inicien els schedules, perque no van precedits de cap altre.
 * 2_Cada cop que trobem un dels nodes inici, el recorrem fins que el node final ja no tingui connexions.
*/
void generate_output(const Graph &graph) {
    vector<vector<int> > schedule;

    int n = (graph.size() - 4) / 2;

    for (int i = n; i < 2 * n; ++i) {
        vector<int> schedule_i;
        schedule_i.push_back(i-n);

        bool new_pilot = true;
        for (int j = n; j < 2*n; ++j){
            if (graph[j][i-n] == 1) new_pilot = false;
        }
        if (new_pilot) {
            generate_schedule_i(graph, i, schedule_i);
            schedule.push_back(schedule_i);
        }
    }

    for (int i = 0; i < schedule.size(); ++i){
        for (int j = 0; j < schedule[i].size(); ++j){
            if (j != 0) cout << ' ';
            cout << schedule[i][j] + 1;
        }
        cout << endl;
    }

    cout << endl << endl << endl << schedule.size() << endl;
}

void print_flow_graph(Graph &graph) {
    cout << "Graph Size: " << graph.size() << endl;
    for (int i = 0; i < graph.size(); ++i) {
        for (int j = 0; j < graph.size(); ++j) {
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
        cout
                << "La versió introduida no es correcta. Siusplau, introdueixi la versió sobre la que vol executar el programa (1 - Versió 1, 2 - Versió 2):"
                << endl;
        cin >> v;
    }

    //Demanem algorisme a executar
    cout << "Siusplau, introdueixi el algorisme a utilitzar (1 - Edmonds-Karp, 2 - Ford-Fulkerson):" << endl;
    int k;
    cin >> k;
    while (k != 1 and k != 2) {
        cout
                << "L'algorisme introduit no es correcte. Siusplau, introdueixi l'algorisme sobre el que vol executar el programa (1 - Edmonds-Karp, 2 - Ford-Fulkerson):"
                << endl;
        cin >> k;
    }

    time_t ini = time(0);

    //Llegim la entrada d'un fitxer passat com a paràmetre
    vector<flight> flight_input;
    read_new_input(flight_input);
    unsigned long n = flight_input.size();

    //Convertim la entrada en un graf amb pesos sobre el que aplicar max flow (que serà diferent en funció de si acceptem que els pilots
    //viatjin o no)
    Graph capacity_matrix(2 * n + 4, vector<int>(2 * n + 4, 0));
    Graph flow_matrix(2 * n + 4, vector<int>(2 * n + 4, 0));
    if (v == 1) parse_last_input_to_max_flow_adjacence_list_version_1(flight_input, capacity_matrix);
    else parse_last_input_to_max_flow_adjacence_list_version_2(flight_input, capacity_matrix);

    int r, l, m, f, p;
    Graph best_flow;
    r = n - 1;
    l = 0;

    while (l <= r) {
        m = (r + l) / 2;
        capacity_matrix[2 * n + 2][2 * n] = m;
        capacity_matrix[2 * n + 1][2 * n + 3] = m;
        capacity_matrix[2 * n][2 * n + 1] = m;

        if (k == 1)
            f = EdmondsKarp(capacity_matrix, flow_matrix, 2 * n + 2, 2 * n + 3);
        else
            f = FordFulkerson(capacity_matrix, flow_matrix, 2 * n + 2, 2 * n + 3);

        if (f == n + m) {
            best_flow = flow_matrix;
            p = m;
            r = m - 1;
        } else l = m + 1;

        for (int i = 0; i < 2 * n + 4; ++i) {
            for (int j = 0; j < 2 * n + 4; ++j) {
                flow_matrix[i][j] = 0;
            }
        }
    }


    time_t one = time(0);

    //print_flow_graph(best_flow);

    for (int i = n; i < 2*n; ++i){
        for (int j = 0; j < n; ++j){
            if (best_flow[i][j] == 1) cout << "reaprovechando supaipa" << endl;
        }
    }

    cout << "MAX FLOW: " << f << endl;
    cout << "PILOTOS: " << p << endl;
    cout << "Algoritmo ejecutado!" << endl;

    cout << time(0) - one << " " << one - ini << endl;

    generate_output(best_flow);
}