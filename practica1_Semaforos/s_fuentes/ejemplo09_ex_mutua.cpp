// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplo 9 (ejemplo09_exclusion_mutua.cpp)
//
// Problema de Productor / Consumidor con una escritura 
// y una lectura  
//
// Queremos evitar la interfolacion L , E (primmero leer , luego escribir)
// SOLO SE PERMITE LA INTERFOLACIÓN E , L (El productor escribe , 
//                                           luego el consumidor lee )
//
// Por tanto en cualquier estado: 
//  0 <= #E <= #L (el numero de )
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
using namespace SEM;

/*
* Variable compartida
    -La hebra productora genera un valor en ella
    -La hebra consumidora lee ese valor
*/
int compartida ;

thread productor ,
       consumidor ;

Semaphore puede_consumir = 0 ,
          puede_producir = 1; 


template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

int generarValor(){
    int valor_producido = aleatorio <10 , 100> ();
    cout<<"Producido : \t"<<valor_producido<<endl;
    return valor_producido;
}

void funcion_productor(){
    //var. local para almacenar el valor generado
    int valor;
    for (int i = 0 ; i < 10 ; ++i){
        valor = generarValor(); //genera valor
        sem_wait(puede_producir); 
        compartida = valor;
        sem_signal(puede_consumir);
    }
}

void funcion_consumidor(){
    //var. local para almacenar el valor compar
    int valor2;
    for (int i = 0 ; i < 10 ; ++i){
        /*
            *Decrementamos valor semaforo , 
            *(#L aparece con signo negativo en la expresión #E-#L)
            *Se tiene que hacer antes de consumir el valor para que
            *el consumidor espere si es necesario y asi evitar que 
            *la expresión  #E - #L tome valor negativo
            *
        */
        sem_wait(puede_consumir); 
        valor2 =  compartida;  // #L (consumir valor)
        cout << "\tConsumido\t" << valor2 << endl;
        sem_signal(puede_producir);
    }
}


int main(){

    productor = thread ( funcion_productor );
    consumidor = thread ( funcion_consumidor );

    productor.join();
    consumidor.join();

    return 0;
}