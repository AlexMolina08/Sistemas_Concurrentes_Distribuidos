// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplo 6 (ejemplo06_atomicos.cpp)
// Uso de un tipo atómico para incrementos concurrentes de un entero
//
// Historial:
// Creado en Mayo de 2017
// Modificado en octubre de 2020
// -----------------------------------------------------------------------------


#include <iostream>
#include <thread>
#include <atomic>
using namespace std;

const long num_iter = 1000000l;
atomic<int> contador_atomico;
int contador_no_atomico;

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
    for (int i = 0 ; i < num_iter ; ++i )
        contador_atomico += 2; 
}

/*
    *Funcion que incrementa en 1M el contador no atómico
*/
void funcion_hebra_no_atomica(){
    for (int i = 0 ; i < num_iter ; ++i )
        contador_no_atomico += 2; 
}

int main(){

    ; contador_no_atomico = 0;
    //lanzamos hebras
    hebra_atomic_1 = thread ( funcion_hebra_atomica ); 
    hebra_atomic_2 = thread ( funcion_hebra_atomica );
    hebra_atomic_3 = thread ( funcion_hebra_atomica );
    hebra_atomic_1.join();
    hebra_atomic_2.join();
    hebra_atomic_3.join();


    hebra_no_atomic_1 = thread ( funcion_hebra_no_atomica );
    hebra_no_atomic_2 = thread ( funcion_hebra_no_atomica );
    hebra_no_atomic_3 = thread ( funcion_hebra_no_atomica );    
    hebra_no_atomic_1.join();
    hebra_no_atomic_2.join();
    hebra_no_atomic_3.join();

    cout<<"\n***Resultados***\n";
    cout<<"\tContador atómico : "<<contador_atomico<<endl;
    cout<<"\tContador no atómico : "<<contador_no_atomico<<endl;

    return 0;
}