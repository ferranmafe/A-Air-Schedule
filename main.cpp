#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include <fstream>
#include <sstream>
#include <stdlib.h>
using namespace std;

struct edge {
  int arriving_vertex;
  int weight;
};

struct vertex {
  int id;
  list<edge> outgoing_edges;
};

vector<vector<int> > read_new_input() {
  //Generem una matriu buida, que es la que retornarem plena amb els vols a tractar
  vector<vector<int> > flights_info_vector;

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
      vector<int> new_flight;
      istringstream line(s);
      for (string str; line >> str;) new_flight.push_back(atoi(str.c_str()));
      flights_info_vector.push_back(new_flight);
    }
    flight_file.close();
    return flights_info_vector;
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
*/
vector<vertex> parse_last_input_to_max_flow_adjacence_list_version_1(const vector<vector<int> >& flight_input) {
  //Inicialitzem el graf de sortida
  vector<vertex> outgoing_graph(2 * flight_input.size() + 2);

  //Definim els id dels nodes S (-1) i T (-2)
  outgoing_graph[2 * flight_input.size()].id = -1;
  outgoing_graph[2 * flight_input.size() + 1].id = -2;

  //Recorrem el graf de sortida. Per a cada element, li indiquem quin ID te, i fem
  //les connexions amb t
  for (int i = 0; i < 2 * flight_input.size(); ++i) {
    //Afegim l'ID del vol (aixi podrem recuperar la relació vol - vertex a la sortida)
    outgoing_graph[i].id = i % flight_input.size();

    //En el cas dels vertex del grup B (els que arriben a t) a més de l'ID del vol
    //afegim la aresta que arriba a t
    if (i >= flight_input.size()) {
      edge t_edge;
      t_edge.arriving_vertex = 2 * flight_input.size() + 1;
      t_edge.weight = 1;
      outgoing_graph[i].outgoing_edges.insert(outgoing_graph[i].outgoing_edges.begin(), t_edge);
    }
  }

  //Connectem s a tots els vertex del grup A
  for (int i = 0; i < flight_input.size(); ++i) {
    edge s_edge;
    s_edge.arriving_vertex = i;
    s_edge.weight = 1;
    outgoing_graph[2 * flight_input.size()].outgoing_edges.insert(outgoing_graph[2 * flight_input.size()].outgoing_edges.begin(), s_edge);
  }

  //Recorrem tots els elements del grup A i per cada vol fem la connexió entre grup
  //A i grup B si (Vol destí A = Vol origen B i temps arribada A + 15 <= temps sortida B)
  for (int i = 0; i < flight_input.size(); ++i) {
    for (int j = 0; j < flight_input.size(); ++j) {
      if (flight_input[i][1] == flight_input[j][0] && flight_input[i][3] + 15 <= flight_input[j][2]) {
        edge edge_ij;
        edge_ij.arriving_vertex = j + flight_input.size();
        edge_ij.weight = 1;
        outgoing_graph[i].outgoing_edges.insert(outgoing_graph[i].outgoing_edges.begin(), edge_ij);
      }
    }
  }
  return outgoing_graph;
}

/*
vector< list< pair<int, int> > > parse_last_input_to_max_flow_adjacence_list_version_2(const& vector<vector<int> > flight_input) {

}
*/

/* PROCEDURE:
 * 1_Recorrem el graf buit buscant s
 * 2_Per cada vertex connectat amb s, mirem si te alguna connexió o no (si la té va al schedule com a pair)
 * 3_Mirem el grup B. Aquells vertex que no tinguin cap aresta incident serán pilots nous. Ho afegim al
 * schedule com a pair (inici, buit)
 * 4_Juntem les parelles de vols connectats formant el schedule de cada pilot.
 * 5_Retornem el length del schedule com a número de pilots, i després imprimim el schedule
*/
void generate_schedule_and_output(const vector<vertex>& graph, const vector<vector<int> >& initial_flights) {
  vector<int> connected_flights(initial_flights.size());
  vector<bool> flights_without_another_one(initial_flights.size());

  //Busquem s, i el seu index quedarà guardat a i.
  bool s_found = false;
  int i = graph.size() - 1;
  while(i >=0 and not s_found) {
    if (graph[i].id = -1) s_found = true;
    else --i;
  }

  //Recorrem les arestes que surten de s, i per les que estàn connectades a una altre
  //ens guardem la relació
  list<edge>::iterator it = graph[i].outgoing_edges.begin();
  for (it, it != graph[i].outgoing_edges.end(); ++it) {
    if (graph[it->arriving_vertex].outgoing_edges.begin() != graph[it->arriving_vertex].outgoing_edges.end()) {
      connected_flights[it->arriving_vertex] = graph[it->arriving_vertex].outgoing_edges.begin().arriving_vertex;
      flights_found[it->arriving_vertex] = true;
    }
  }

  //Afegim els vols tq no es poden concatenar amb cap altre
  for (int j = 0; i < flights_found.size(); ++j) {
    if (!flights_found[i]) {
      connected_flights[i] = -1;
    }
  }

  vector<vector<int> > scheduled_flights;
  for (int i = 0; i < connected_flights.size(); ++i) {

  }
}

void print_flow_graph(vector<vertex>& graph) {
  cout << "Graph Size: " << graph.size() << endl;
  for (int i = 0; i < graph.size(); ++i) {
    cout << "Vertex " << i << endl;
    cout << "ID: " << graph[i].id << endl;
    list<edge>::iterator it = graph[i].outgoing_edges.begin();
    for (it; it != graph[i].outgoing_edges.end(); ++it) {
      cout << "Ingoing vertex: " << it->arriving_vertex << " Weight: " << it->weight << endl;
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
  vector<vector<int> > flight_input = read_new_input();

  //Convertim la entrada en un graf amb pesos sobre el que aplicar max flow (que serà diferent en funció de si acceptem que els pilots
  //viatjin o no)
  vector<vertex> graph_to_apply_max_flow;
  if (v == 1) graph_to_apply_max_flow =  parse_last_input_to_max_flow_adjacence_list_version_1(flight_input);
  //else graph_to_apply_max_flow = parse_last_input_to_max_flow_adjacence_list_version_1();

}
