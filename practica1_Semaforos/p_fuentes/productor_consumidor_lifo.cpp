// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Practica 1. Sincronización de hebras con semáforos.
//
// Problema del Productor-Consumidor con semáforos (productor_consumidor.cpp)
// Solución LIFO con semáforos al problema del productor-consumidor
//
//  Cada item producido debe ser leido ( ningún item se pierde )
//  Ningún item se lee más de una sola vez
//  
//  Compilar con:
//      g++ -o bin/productor_consumidor_lifo -pthread -std=c++11 productor_consumidor_lifo.cpp Semaphore.cpp -ISemaphore
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


const int num_items = 10 , //numero de items a generar
          tam_vec = 5; //tam.del vector



thread productor1 , consumidor1 ;

//Variable global que nos dice cual es la primera celda libre para producir
//y cual es la ultima celda en la que se ha producido 
// (que es la anterior a primera_libre)
int primera_libre = 0; 
 

Semaphore libres = tam_vec ,  // tam_vec + #L - #E  , al principio tam_vec
          ocupadas = 0; // #E - #L . al principio 0

int vec[tam_vec] = {0};

mutex buffer; //mutex para asegurar exclusion mutua

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

int producir_dato()
{
   static int contador = 0 ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "producido: " << contador << endl << flush ;

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

void  funcion_hebra_productora()
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato = producir_dato() ;
      //Paramos la hebra hasta que haya celdas libres en la cola de libres
      //del semaforo (al inicio entra siempre)
      sem_wait(libres);
      buffer.lock(); 
      vec[primera_libre] = dato;
      buffer.unlock();
      cout<<"\tLibres "<<tam_vec - (primera_libre + 1)<<" de "<<tam_vec<<endl<<flush;
      primera_libre ++ ;
      sem_signal(ocupadas);

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
      buffer.lock();
      dato = vec[primera_libre - 1]; //consumimos ultimo dato en producirse
      buffer.unlock();
      primera_libre --;
      cout<<"\tLibres "<<tam_vec - (primera_libre + 1)<<" de "<<tam_vec<<endl<<flush;
      sem_signal(libres);

      consumir_dato( dato ) ;
    }
}
//----------------------------------------------------------------------

int main(){

   productor1 = thread(funcion_hebra_productora);
   consumidor1 = thread(funcion_hebra_consumidora);

   productor1.join();
   consumidor1.join();


   test_contadores();

   return 0;
}