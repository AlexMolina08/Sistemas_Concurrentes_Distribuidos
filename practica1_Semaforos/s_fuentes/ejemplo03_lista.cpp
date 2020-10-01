// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplo 3  (ejemplo06 y ejemplo07 de las diapositivas del seminario)
// Creación de un vector de hebras , cada una imprime el factorial de un numero
// Creación de un vector de futuros
//
// Historial:
// Creado en Abril de 2017
// -----------------------------------------------------------------------------

#include <iostream>
#include <thread>     // declaracion de {\bf std::thread}, {\bf std::async}, {\bf std::future}
#include <future>
using namespace std ; // permite acortar la notación (abc en lugar de std::abc)
const int num_hebras = 8;
thread hebras[num_hebras];
future<long> futuros[num_hebras];
mutex m; //para hacer e.m en el cout



// declaración de la función {\bf factorial} (parámetro {\bf int}, resultado {\bf long})
long factorial( int n ) { return n > 0 ? n*factorial(n-1) : 1 ; }

void funcion_hebra(int i){
  long result = factorial(i);
  m.lock();
  cout<<"Hebra "<<i<<" || Factorial: "<<result<<flush<<endl;
  m.unlock();
}


int main(){


  //--- EJECUCION DE VECTOR DE HEBRAS ---
    cout<<"\n--- vector de threads ---\n";
  //Lanzamos todas las hebras de la lista
  for(int i = 0 ; i < num_hebras ; i++){
    hebras[i] = thread(funcion_hebra , i);
  }
  //Esperamos a que todas terminen 
  for(int i = 0 ; i < num_hebras ; i++)
    hebras[i].join();

  // --- EJECUCION VECTOR DE FUTUROS ----

  //--- Lanzamos los futuros
  cout<<"\n--- vector de futuros ---\n";
  for(int j = 0 ; j < num_hebras ; ++j)
    futuros[j] = async (launch::async , factorial , j);
  //--- obtenemos el resultado de los futuros
  for(int j = 0 ; j < num_hebras ; ++j)
    cout<<"Hebra "<<j<<" || Factorial: "<<futuros[j].get()<<flush<<endl;
  return 0;
}