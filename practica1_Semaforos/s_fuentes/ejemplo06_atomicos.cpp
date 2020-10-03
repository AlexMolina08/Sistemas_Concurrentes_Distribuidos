// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplo 6 (ejemplo06_atomicos.cpp)
// Uso de un tipo atómico para incrementos concurrentes de un entero
//
// El tipo no atómico tarda menos porque las operaciones no se hacen en exclusión
// mutua y esto implica que muchas se hacen a la vez , pero acarrea que
// el resultado es incorrecto
//
// compilar con : 
// 
// g++ -std=c++11 -pthread -o bin/ejemplo06_atomicos_exe ejemplo06_atomicos.cpp
// 
//
// Historial:
// Creado en Mayo de 2017
// Modificado en octubre de 2020
// -----------------------------------------------------------------------------


#include <iostream>
#include <thread>
#include <atomic>

using namespace std;
using namespace std::chrono;

const int num_threads = 3; //num_threads de cada tipo
const long num_iter = 1000000l;
atomic<int> contador_atomico;
int contador_no_atomico;


void incrementa(int &contador){
    contador++;
}

//creación hebras 
thread hebra_atomic_1 , 
       hebra_atomic_2 ,
       hebra_atomic_3 ,
       hebra_no_atomic_1 ,
       hebra_no_atomic_2 ,
       hebra_no_atomic_3 ;

/*
    *Funcion que incrementa en 1M el contador atómico
*/
void funcion_hebra_atomica(){
    for (int i = 0 ; i < num_iter ; ++i ) {
        contador_atomico ++;
    }
}

/*
    *Funcion que incrementa en 1M el contador no atómico
*/
void funcion_hebra_no_atomica(){
    for (int i = 0 ; i < num_iter ; ++i ){
        contador_no_atomico ++;
    }
}

int main(){

    ; contador_no_atomico = 0;
    //lanzamos hebras

    time_point<steady_clock> inicio_atom = steady_clock::now();

    hebra_atomic_1 = thread ( funcion_hebra_atomica ); 
    hebra_atomic_2 = thread ( funcion_hebra_atomica );
    hebra_atomic_3 = thread ( funcion_hebra_atomica );
    hebra_atomic_1.join();
    hebra_atomic_2.join();
    hebra_atomic_3.join();

    time_point<steady_clock> fin_atom = steady_clock::now();


    time_point<steady_clock> inicio_no_atom = steady_clock::now();


    hebra_no_atomic_1 = thread ( funcion_hebra_no_atomica );
    hebra_no_atomic_2 = thread ( funcion_hebra_no_atomica );
    hebra_no_atomic_3 = thread ( funcion_hebra_no_atomica );    
    hebra_no_atomic_1.join();
    hebra_no_atomic_2.join();
    hebra_no_atomic_3.join();

    time_point<steady_clock> fin_no_atom = steady_clock::now();


    duration<float,milli> tiempo_atom = fin_atom - inicio_atom ,
                         tiempo_no_atom = fin_no_atom - inicio_no_atom ; 

    cout <<"\n***Resultados***\n"
         << "\tEsperado : "<<num_threads * num_iter << endl 
         << "\tContador atómico : " << contador_atomico<< endl
         << "\tContador no atómico : " << contador_no_atomico << endl
         << "\tTiempo atómico : " << tiempo_atom.count() << "milisegundos \n"
         << "\tTiempo no atómico : " << tiempo_no_atom.count() << "milisegundos \n";        
         

    return 0;
}