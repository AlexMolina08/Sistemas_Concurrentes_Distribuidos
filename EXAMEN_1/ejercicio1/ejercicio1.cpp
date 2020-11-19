
#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;


mutex pantalla;
const int num_fumadores = 3;
const int num_ingredientes = 3;  //ingredientes distintos que produce el estanquero

//Creacion Vector de hebras de fumadores y hebra estanquero
thread fumadores[num_fumadores] , 
       estanquero ;

const int tam_buffer = 3;

thread suministradora[2];

int buffer[tam_buffer];

//Variables globales que nos dicen cual es la primera celda libre para producir
//y cual es la ultima celda en la que se ha producido 
int primera_libre = 0 , //posicion del productor
    primera_ocupada = 0; //posicion del consumidor
    

//Vector de semáforos (cada posicion es un tipo de ingrediente distinto)
Semaphore ingrediente[num_ingredientes] = {0,0,0};
Semaphore mostrador_libre = 1;

Semaphore libres = tam_buffer ,  // tam_vec + #L - #E  , al principio tam_buffer
          ocupadas = 0; // #E - #L . al principio 0

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
//LA EJECUTAN LAS HEBRAS SUMINISTRADORAS
int producir_ingrediente()
{
   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_produ( aleatorio<10,100>() );

   // informa de que comienza a producir
   cout << "👷 Suministrador : empieza a producir ingrediente (" << duracion_produ.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_produ' milisegundos
   this_thread::sleep_for( duracion_produ );

   //GENERA INGREDIENTE ALEATORIO (ENTRE 0 y el num de fumadores)
   const int num_ingrediente = aleatorio<0,num_fumadores-1>() ;

   // informa de que ha terminado de producir
   cout << "👷 Suministrador : termina de producir ingrediente " << num_ingrediente << endl;

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
// coge un valor del buffer 

void funcion_hebra_estanquero(  )
{
   int ingrediente_producido;
   while(true){
      //el estanquero espera a que haya algun valor en el buffer
      sem_wait(ocupadas);
      ingrediente_producido = buffer[primera_ocupada % tam_buffer];
      primera_ocupada ++;
      sem_signal(libres);
      
      sem_signal(ingrediente[ingrediente_producido]);
   }

}

//----------------------------------------------------------------------
// función que ejecutan las hebras suministradora 

void funcion_hebra_suministradora(int id){
   int ingrediente_producido;

   while(true){

      int dato = producir_ingrediente() ;
      int aux;

      sem_wait(libres);
      buffer[primera_libre%tam_buffer] = dato; // escribe en dato en la primera celda libre
      primera_libre ++ ;
      aux = primera_libre;

      sem_signal(ocupadas); //Incrementamos el valor de ocupadas en 1 

      pantalla.lock();
      cout<<"\n\n SUMINISTRADORA "<<id<<" produce "<<dato<<endl;
      pantalla.unlock();

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

   for (int i = 0 ; i < 2 ; ++i) 
      suministradora[i] = thread(funcion_hebra_suministradora , i);
   
   
   estanquero = thread(funcion_hebra_estanquero);

   for (int i = 0 ; i < num_fumadores ; ++i)
      fumadores[i].join();
   estanquero.join();


}
