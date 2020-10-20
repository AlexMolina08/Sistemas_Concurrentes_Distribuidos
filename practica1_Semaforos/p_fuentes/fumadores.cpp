// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Practica 1. Sincronización de hebras con semáforos.
//
// Problema de los fumadores con semáforos (fumadores.cpp)
// 
//
//Descripción:
//    * Estanquero pone un producto en el mostrador (0 , 1 ó 2 - representan papel , tabaco , 
//       cerillas -)
//    * A cada fumador le falta un producto distinto. Cada fumador espera a que el estanquero
//      ponga el producto para fumar
//  
//  Compilar con:
//      g++ -o bin/fumadores -pthread -std=c++11 fumadores.cpp Semaphore.cpp -ISemaphore
//
// Historial:
// Creado en Octubre de 2020
// -----------------------------------------------------------------------------

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

const int num_fumadores = 3;
const int num_ingredientes = 3;  //ingredientes distintos que produce el estanquero

//Creacion Vector de hebras de fumadores y hebra estanquero
thread fumadores[num_fumadores] , 
       estanquero;

//Vector de semáforos (cada posicion es un tipo de ingrediente distinto)
Semaphore ingrediente[num_ingredientes] = {0,0,0};
Semaphore mostrador_libre = 1;

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

//-------------------------------------------------------------------------
// Función que simula la acción de producir un ingrediente, como un retardo
// aleatorio de la hebra (devuelve número de ingrediente producido)

int producir_ingrediente()
{
   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_produ( aleatorio<10,100>() );

   // informa de que comienza a producir
   cout << "👷 Estanquero : empieza a producir ingrediente (" << duracion_produ.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_produ' milisegundos
   this_thread::sleep_for( duracion_produ );

   //GENERA INGREDIENTE ALEATORIO (ENTRE 0 y el num de fumadores)
   const int num_ingrediente = aleatorio<0,num_fumadores-1>() ;

   // informa de que ha terminado de producir
   cout << "👷 Estanquero : termina de producir ingrediente " << num_ingrediente << endl;

   return num_ingrediente ;
}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "\t 🚬 Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "\t 🚬 Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}


//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero(  )
{
   int ingrediente_producido;
   while(true){
      //el estanquero espera a que el mostrador quede libre (al inicio siemopre entra)
      sem_wait(mostrador_libre);
      ingrediente_producido = producir_ingrediente();
      sem_signal(ingrediente[ingrediente_producido]);
   }

}

void retirarIngrediente(int num_fumador){
    cout << "\t 🤝 🚬 Fumador " << num_fumador << "  :"
          << " RETIRA INGREDIENTE DEL MOSTRADOR " << endl;
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true )
   {
      //el fumador espera a su ingrediente (en este caso coincide con el num_fumador)
      sem_wait(ingrediente[num_fumador]);

      retirarIngrediente(num_fumador); //informar que el fumador ha retirado el ingrediente

      //tras fumar , informa al estanquero que el mostrador queda libre
      sem_signal(mostrador_libre);

      //una vez que tiene el ingrediente , no tiene que esperar a nada , fuma 
      //y cuando termine vuelve a esperar a por su ingrediente
      fumar(num_fumador); 
   }
}


//----------------------------------------------------------------------

int main()
{

   for (int i = 0 ; i < num_fumadores ; ++i)
      fumadores[i] = thread(funcion_hebra_fumador , i);
   estanquero = thread(funcion_hebra_estanquero);

   for (int i = 0 ; i < num_fumadores ; ++i)
      fumadores[i].join();
   estanquero.join();


}
