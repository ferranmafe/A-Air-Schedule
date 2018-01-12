#include <iostream>
#include <stdlib.h>
using namespace std;

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
  string entrada = argv[3];
}
