// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplo 6 (ejemplo06_atomicos.cpp)
// Uso de un tipo atómico para incrementos concurrentes de un entero
//
// Problema de la espera única ( Productor / Consumidor con una escritura 
// y una lectura ) 
//
// Queremos evitar la interfolacion L , E (primmero leer , luego escribir)
// SOLO SE PERMITE LA INTERFOLACIÓN E , L (El productor escribe , 
//                                           luego el consumidor lee )
//
// Por tanto en cualquier estado: 
//  0 <= #E <= #L
//
// compilar con : 
// 
// g++ -std=c++11 -pthread -o bin/ejemplo06_atomicos_exe ejemplo06_atomicos.cpp
// 
//
// Historial:
// Creado en octubre de 2020
// -----------------------------------------------------------------------------


#include <iostream>
#include <cassert>
#include <thread>
#include "Semaphore.h"
#include <random> // dispositivos, generadores y distribuciones aleatorias

using namespace std;

/*
* Variable compartida
    -La hebra productora genera un valor en ella
    -La hebra consumidora lee ese valor
*/
int compartida ;

thread productor ,
       consumidor ;


template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

int produceValor(){
    return aleatorio <10 , 100> ();
}

void funcion_productor(){
    //var. local para almacenar el valor generado
    int valor;
    while (true){
        valor = produceValor(); // #E
        compartida = valor;
        cout << "Productor produce" << valor << endl;
    }
}

void funcion_consumidor(){
    //var. local para almacenar el valor compar
    int valor2;
    while(true){
        valor2 =  = compartida;  // #L
        cout << "\tConsumidor consume" << valor2 << endl;
    }
}


int main(){

    productor = thread ( funcion_productor );
    consumidor = thread ( funcion_consumidor );

    productor.join();
    consumidor.join();

    return 0;
}