//
// Created by Sebastián Sánchez Menéndez on 12/1/18.
//

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

void parse_last_input_to_max_flow_adjacence_list_version_2(const vector<flight> &flight_input, Graph &capacity_matrix) {
    //Recorrem el graf de sortida. Fem les connexions amb t
    unsigned long n = flight_input.size();
    vector<bool> visited_vertex(n, false);

    for (int i = 0; i < n; ++i) {
        capacity_matrix[2 * n][i] = 1;
        capacity_matrix[i][2 * n + 3] = 1;
    }

    for (int i = n; i < 2 * n; ++i) {
        capacity_matrix[2 * n + 2][i] = 1;
        capacity_matrix[i][2 * n + 1] = 1;
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
                        capacity_matrix[u+n][j] = 1;
                        for (int k = 0; k < flight_input.size(); ++k) {
                            if (capacity_matrix[j+n][k]) {
                                capacity_matrix[u+n][k] = 1;
                                capacity_matrix[i+n][k] = 1;
                            }
                        }
                    }
                } else {
                    if (flight_input[u][1] == flight_input[j][0] && flight_input[u][3] + 15 < flight_input[j][2]) {
                        capacity_matrix[u+n][j] = 1;
                        capacity_matrix[i+n][j] = 1;
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

vector<vector<int> > generate_output_matrix(const Graph &graph) {
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
    cout << "./program [Versión] [Fichero de entrada]" << endl;
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
    if (argc != 3) usage();
    int version = atoi(argv[1]);
    if (version <= 0 || version >= 3) {
        cout << "La versión introducida no es válida. La versión ha de ser un entero entre 1 y 2." << endl;
        exit(1);
    }
    string fileName = argv[2];
    string pathInputName = baseInputName + fileName + ".air";
    vector<flight> flight_input;
    read_new_input(flight_input, pathInputName);
    Graph capacity_matrix(2 * flight_input.size() + 4, vector<int>(2 * flight_input.size() + 4, 0));
    if (version == 1) parse_last_input_to_max_flow_adjacence_list_version_1(flight_input, capacity_matrix);
    else parse_last_input_to_max_flow_adjacence_list_version_2(flight_input, capacity_matrix);
    int f;
    Graph flow_matrix(2 * flight_input.size() + 4, vector<int>(2 * flight_input.size() + 4, 0));
    f = EdmondsKarp(capacity_matrix, flow_matrix, 2 * flight_input.size()+2,
                    2 * flight_input.size() + 3);
    vector<vector<int> > schedule = generate_output_matrix(flow_matrix);
    string pathScheduleName = baseOutputName + fileName + ".out";
    string scheduleContent = writeSchedule(schedule);
    writeFile(pathScheduleName, scheduleContent);
}