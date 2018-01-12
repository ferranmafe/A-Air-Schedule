#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <climits>
#include <stdlib.h>
#include "algorithms.cpp"
using namespace std;

typedef vector<int> flight;




#include <iostream>
#include <time.h>
#include <stdio.h>


double diffclock(clock_t clock1, clock_t clock2) {
    double diffticks = clock1 - clock2;
    double diffms = (diffticks * 1000) / CLOCKS_PER_SEC;
    return diffms;
}

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

void writeVectorFile(string filePath, vector<string>& fileContent){
    ofstream file;
    file.open (filePath, fstream::in | fstream::out | fstream::trunc);
    for (int i = 0; i < fileContent.size(); i++){
        file << fileContent[i] << endl;
    }
    file.close();
}

void writeTimes(string filePath, vector<double> times){
    ofstream file;
    file.open (filePath, fstream::in | fstream::out | fstream::trunc);
    for (int i = 2; i < times.size(); i++){
        file << "instance_100_" + to_string(i) + "_*.air;" << times[i]/10 << endl;
    }
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

int main() {
    string baseInputName = "./Benchmark/";
    string baseOutputName = "./Output/MaximumBipartiteMatching/";
    vector<string> resultContent1;
    vector<string> resultContent2;
    vector<double> tiempo_v1_ek(31, 0);
    vector<double> tiempo_v2_ek(31, 0);
    vector<double> tiempo_v1_d(31, 0);
    vector<double> tiempo_v2_d(31, 0);

    for (int i = 2; i <= 30; i++) {
        int t = 0;
        for (int j = 1; j <= 10; j++) {
            string fileName = "instance_100_" + to_string(i) + "_" + to_string(j);
            string pathInputName = baseInputName + fileName + ".air";
            vector<flight> flight_input;
            read_new_input(flight_input, pathInputName);
            for (int k = 1; k <= 2; k++) {
                string version = "Version" + to_string(k) + "/";
                //Convertim la entrada en un graf amb pesos sobre el que aplicar max flow (que serà diferent en funció de si acceptem que els pilots
                //viatgin o no)
                for (int l = 0; l < 2; l++) {
                    clock_t begin = clock();
                    //empezar a contar
                    Graph capacity_matrix(2 * flight_input.size() + 2, vector<int>(2 * flight_input.size() + 2, 0));
                    if (k == 1) parse_last_input_to_max_flow_adjacence_list_version_1(flight_input, capacity_matrix);
                    else parse_last_input_to_max_flow_adjacence_list_version_2(flight_input, capacity_matrix);
                    string algorithm_used;
                    if (l == 0)
                        algorithm_used = "EdmondsKarp/";
                    else
                        algorithm_used = "Dinic/";
                    int f;
                    Graph flow_matrix(2 * flight_input.size() + 2, vector<int>(2 * flight_input.size() + 2, 0));
                    if (l == 0)
                        f = EdmondsKarp(capacity_matrix, flow_matrix, 2 * flight_input.size(),
                                        2 * flight_input.size() + 1);
                    else
                        f = DinicAlgorithm(capacity_matrix, flow_matrix, 2 * flight_input.size(),
                                           2 * flight_input.size() + 1);
                    vector<vector<int> > schedule = generate_output_matrix(flow_matrix);
                    //acabar de contar
                    clock_t end = clock();
                    if (l == 0 && k == 1) tiempo_v1_ek[i] += diffclock(end, begin);
                    else if (l == 0 && k == 2) tiempo_v2_ek[i] += diffclock(end, begin);
                    else if (l == 1 && k == 1) tiempo_v1_d[i] += diffclock(end, begin);
                    else if (l == 1 && k == 2) tiempo_v2_d[i] += diffclock(end, begin);

                    string pathScheduleName = baseOutputName + version + algorithm_used + fileName + ".out";
                    if (l == 0 && k == 1) resultContent1.push_back(fileName + ".air " + to_string(schedule.size()));
                    else if (l == 0 && k == 2)
                        resultContent2.push_back(fileName + ".air " + to_string(schedule.size()));
                    string scheduleContent = writeSchedule(schedule);
                    writeFile(pathScheduleName, scheduleContent);
                }
            }
        }
    }
    sort(resultContent1.begin(), resultContent1.end());
    sort(resultContent2.begin(), resultContent2.end());
    writeVectorFile(baseOutputName + "Resultado1.txt", resultContent1);
    writeVectorFile(baseOutputName + "Resultado2.txt", resultContent2);
    writeTimes(baseOutputName + "Times_V1_EK.csv", tiempo_v1_ek);
    writeTimes(baseOutputName + "Times_V2_EK.csv", tiempo_v2_ek);
    writeTimes(baseOutputName + "Times_V1_D.csv", tiempo_v1_d);
    writeTimes(baseOutputName + "Times_V2_D.csv", tiempo_v2_d);
}