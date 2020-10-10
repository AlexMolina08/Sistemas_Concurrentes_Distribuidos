// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Practica 1. Sincronización de hebras con semáforos.
//
// Problema del Productor-Consumidor con semáforos (productor_consumidor.cpp)
// Solución con semáforos al problema del productor-consumidor con múltiples
// productores y consumidores
//
//  Cada item producido debe ser leido ( ningún item se pierde )
//  Ningún item se lee más de una sola vez
//
//  
//  Compilar con:
//   g++ -o bin/productor_consumidor_multi -pthread 
//   -std=c++11 productor_consumidor_multi.cpp Semaphore.cpp -ISemaphore
//
//
// Historial:
// Creado en Octubre de 2020
// -----------------------------------------------------------------------------

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

mutex mtx; 

const int num_items = 10 , //numero de items a generar
          tam_vec = 5;



thread productor1 , consumidor1 , productor2 , consumidor2;

//Variable global que nos dice cual es la primera celda libre para producir
//y cual es la ultima celda en la que se ha producido 
// (que es la anterior a primera_libre)
int primera_libre = 0; 
 

Semaphore libres = tam_vec ,  // tam_vec + #L - #E  , al principio tam_vec
          ocupadas = 0; // #E - #L . al principio 0

int vec[tam_vec] = {0};

//contadores para verificar el programa.
unsigned  cont_prod[num_items] = {0}, // contadores de verificación: producidos
          cont_cons[num_items] = {0}; // contadores de verificación: consumidos

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//**********************************************************************
template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

//********************************************************************** 
// Funcion que devuelve un dato , simulando que tarda un tiempo aleatorio
// en crearlo
//**********************************************************************

int producir_dato(int productorID)
{
   static int contador = 0 ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "productor " << productorID << "| producido: " << contador << endl << flush ;

   cont_prod[contador] ++ ;
   return contador++ ;
}
//----------------------------------------------------------------------

//********************************************************************** 
// Funcion que consume un dato (imprimir por pantalla el dato) , 
// simulando un tiempo aleatorio en ser consumidor
//**********************************************************************
void consumir_dato( unsigned dato )
{
   assert( dato < num_items );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "                  consumido: " << dato << endl ;

} 


//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------


//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {  cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}

//----------------------------------------------------------------------

void  funcion_hebra_productora(int productorID)
{
   for( unsigned i = 0 ; i < num_items / 2 ; i++ )
   {
   
      int dato = producir_dato(productorID) ;
      //Paramos la hebra hasta que haya celdas libres en la cola de libres
      //del semaforo
      sem_wait(libres);
      vec[primera_libre] = dato;
      //cout<<"\tPRODUCTOR PRODUCE"<<vec[primera_libre]<<endl<<flush;
      mtx.lock();
      primera_libre ++ ;
      mtx.unlock();
      sem_signal(ocupadas);
      //Incrementamos el valor de ocupadas en 1 

   }
}

//----------------------------------------------------------------------

void funcion_hebra_consumidora(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato ;
      // Esperar a que haya hebras ocupadas
      sem_wait(ocupadas);
      dato = vec[primera_libre - 1]; //consumimos ultimo dato en producirse
      //cout<<"\tCONSUMIDOR CONSUME"<<vec[primera_libre - 1]<<endl<<flush;
      primera_libre --;
      sem_signal(libres);

      consumir_dato( dato ) ;
    }
}
//----------------------------------------------------------------------

int main(){

   productor1 = thread(funcion_hebra_productora , 1);
   productor2 = thread(funcion_hebra_productora , 2);
   consumidor1 = thread(funcion_hebra_consumidora);

   productor1.join();
   productor2.join();
   consumidor1.join();


   test_contadores();

   return 0;
}