#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <climits>
#include <stdlib.h>
#include "algorithms.cpp"
#include <time.h>
#include <stdio.h>
using namespace std;

typedef vector<int> flight;

void read_new_input(vector<flight>& flights_info_vector, string file_name) {
    //Obrim el fitxer
    fstream flight_file(file_name);
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
void parse_last_input_to_max_flow_adjacence_list_version_1(const vector<flight>& flight_input, Graph& capacity_matrix) {
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

    //Recorrem tots els elements del grup A i per cada vol fem la connexió entre grup
    //A i grup B si (Vol destí A = Vol origen B i temps arribada A + 15 <= temps sortida B)
    for (int i = 0; i < flight_input.size(); ++i) {
        for (int j = 0; j < flight_input.size(); ++j) {
            if (flight_input[i][1] == flight_input[j][0] && flight_input[i][3] + 15 <= flight_input[j][2]) {
                capacity_matrix[i][flight_input.size() + j] = 1;
            }
        }
    }
}

void parse_last_input_to_max_flow_adjacence_list_version_2(const vector<flight>& flight_input, Graph& capacity_matrix) {
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

void generate_schedule_i(const Graph& graph, const int& i, vector<int>& schedule_i) {
    int j = (graph.size() - 2) / 2;
    bool connection_found = false;
    while(j < (graph.size() - 2) && !connection_found) {
        if (graph[i][j] == 1) {
            connection_found = true;
            int k = j%((graph.size() - 2) / 2);
            schedule_i.push_back(k + 1);
            generate_schedule_i(graph, k, schedule_i);
        }
        ++j;
    }
}

/* PROCEDURE:
 * 1_Recorrem les connexions de cada node del grup B amb t. Aquells nodes que no tinguin fluxe seran
 * els que inicien els schedules, perque no van precedits de cap altre.
 * 2_Cada cop que trobem un dels nodes inici, el recorrem fins que el node final ja no tingui connexions.
*/
vector<vector<int> > generate_output_matrix(const Graph& graph) {
    vector<vector<int> > schedule;
    //Recorrem tots els nodes de B. Per cada node que no tingui connexió amb t, busquem el schedule
    for (int i = ((graph.size() - 2) / 2); i < graph.size() - 2; ++i) {
        if (graph[i][graph.size() - 1] != 1) {
            vector<int> schedule_i;
            int j = (i%((graph.size() - 2) / 2));
            schedule_i.push_back(j + 1);
            generate_schedule_i(graph, j, schedule_i);
            schedule.push_back(schedule_i);
        }
    }
    return schedule;
}

void writeFile(string filePath, string fileContent){
    ofstream file;
    file.open (filePath, fstream::in | fstream::out | fstream::trunc);
    file << fileContent;
    file.close();
}

string writeSchedule(vector<vector<int> >& schedule){
    string scheduleInfo = "";
    scheduleInfo += to_string(schedule.size()) + "\n";
    for (int i = 0; i < schedule.size(); ++i) {
        for(int j = 0; j < schedule[i].size(); ++j) {
            scheduleInfo += to_string(schedule[i][j]);
            if (j != schedule[i].size() - 1) scheduleInfo += " ";
        }
        scheduleInfo += "\n";
    }
    return scheduleInfo;
}

void usage() {
    cout << "./program  [Algoritmo de Max Flow] [Versión] [Fichero de entrada]" << endl;
    cout << "Programa que calcula, dados unos vuelos de entrada, el schedule que requiere el mínimo de pilotos para llevarse a cabo" << endl;
    cout << "[Algoritmo de Max Flow] Algoritmo que se va a usar para calcular el grafo de flujos:" << endl;
    cout << "0 -> Edmonds-Karp" << endl;
    cout << "1 -> Dinic" << endl;
    cout << "[Versión] Versión de la práctica que se va a utilizar [1 o 2]" << endl;
    cout << "[Fichero de entrada] Nombre del fichero que contiene los datos de la entrada" << endl;
    exit(1);
}

int main(int argc, char* argv[] ) {
    string baseInputName = "./Benchmark/";
    string baseOutputName = "./";
    if (argc != 4) usage();
    int version = atoi(argv[2]);
    if (version <= 0 || version >= 3) {
        cout << "La versión introducida no es válida. La versión ha de ser un entero entre 1 y 2." << endl;
        exit(1);
    }
    int algoritmo = atoi(argv[1]);
    if (algoritmo < 0 || algoritmo > 1) {
        cout << "El algoritmo introducido no es válido. El algoritmo ha de ser un entero entre 0 y 1." << endl;
        exit(1);
    }
    string fileName = argv[3];
    string pathInputName = baseInputName + fileName + ".air";
    vector<flight> flight_input;
    read_new_input(flight_input, pathInputName);
    Graph capacity_matrix(2 * flight_input.size() + 2, vector<int>(2 * flight_input.size() + 2, 0));
    if (version == 1) parse_last_input_to_max_flow_adjacence_list_version_1(flight_input, capacity_matrix);
    else parse_last_input_to_max_flow_adjacence_list_version_2(flight_input, capacity_matrix);
    int f;
    Graph flow_matrix(2 * flight_input.size() + 2, vector<int>(2 * flight_input.size() + 2, 0));
    if (algoritmo == 0)
        f = EdmondsKarp(capacity_matrix, flow_matrix, 2 * flight_input.size(),
                        2 * flight_input.size() + 1);
    else
        f = DinicAlgorithm(capacity_matrix, flow_matrix, 2 * flight_input.size(),
                           2 * flight_input.size() + 1);
    vector<vector<int> > schedule = generate_output_matrix(flow_matrix);
    string pathScheduleName = baseOutputName + fileName + ".out";
    string scheduleContent = writeSchedule(schedule);
    writeFile(pathScheduleName, scheduleContent);
}
