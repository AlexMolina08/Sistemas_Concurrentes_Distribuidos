// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Seminario 2. Introducción a los monitores en C++11.
//
// Compilar con : g++ -pthread -std=c++11 -o bin/prod_cons_monitor_v2 src/prod_cons_monitor_v2.cpp 
//
// SOLUCION PROD_CONS CON MONITOR SC (Señalar y continuar)
// -----------------------------------------------------------------------------

//********************************************************************** 
// Funcion que devuelve un dato , simulando que tarda un tiempo aleatorio
// en crearlo
//**********************************************************************


#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include <condition_variable>

using namespace std;


const int num_items = 50 , 
          num_productores = 10 , 
          num_consumidores = 10 , 
          
          items_productor = num_items / num_productores ,
          items_consumidor = num_items / num_consumidores
          ;

//Array compartido que indica en cada momento cuantos elementos se han
//producido ya
//CADA CELDA REPRESENTA EL NUMERO DE ELEMENTOS PRODUCIDOS
int producidos[num_productores] = {0}; 

thread productor[num_productores] , 
       consumidor[num_consumidores];


unsigned
   cont_prod[num_items], // contadores de verificación: producidos
   cont_cons[num_items]; // contadores de verificación: consumidos



/**************************************************************************/

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." << flush ;

   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      if ( cont_prod[i] != 1 )
      {
         cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {
         cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}


/**************************************************************************/

int producir_dato(int ID_productor)
{
   //comprobamos que el ID_productor es correcto
   //assert(ID_productor >= 0 && ID_productor < num_productores);
   
   int contador = ID_productor*items_productor +  producidos[ID_productor];
   cout<<producidos[ID_productor]<<endl;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
   
   cout << "productor"<<ID_productor <<"|| producido: " << contador << endl << flush ;

   
   cont_prod[contador] ++ ;

   producidos[ID_productor]++; //Incrementamos en 1 el contador de el productor 
   return contador ;
}
//----------------------------------------------------------------------

//********************************************************************** 
// Funcion que consume un dato (imprimir por pantalla el dato) , 
// simulando un tiempo aleatorio en ser consumidor
//**********************************************************************
void consumir_dato( unsigned dato )
{
   //assert( dato < num_items );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "                  consumido: " << dato << endl ;

} 

/**************************************************************************/

class ProdConsSC{

    private:
    static const int tam_vec = 5 ; //numero entradas del buffer
                     
    int num_productores , num_consumidores;

    mutex cerrojo;

    condition_variable libres ,  // cola donde espera los consumidores
                       ocupadas ; //cola donde espera los productores
    
    int vec[tam_vec]; 

    int primera_libre; 
    

    public:
    ProdConsSC(int nProductores , int nConsumidores);
    void insertar(int valor);
    int extraer();

};


ProdConsSC::ProdConsSC(int nProductores , int nConsumidores){
    primera_libre = 0 ;
    num_productores = nProductores ; num_consumidores = nConsumidores;
}

void ProdConsSC::insertar(int valor){
    unique_lock<mutex> guarda(cerrojo); //ganamos e.m

    //si están todas las celdas ocupadas , la hebra productor se bloquea
    //en la cola
    while(primera_libre == tam_vec) { 
        libres.wait(guarda);
    }

    vec[primera_libre] = valor; //insertamos el valor en la primera celda libre
    primera_libre ++;

    //Avisamos a todos los productores que la celda
    for(int i = 0 ; i < num_productores ; ++i)
        ocupadas.notify_one(); 


}

int ProdConsSC::extraer(){
    unique_lock<mutex> guarda(cerrojo); //ganamos e.m

    //si no hay datos para leer aún , el consumidor se bloquea en la cola 
    //de libres

    while(primera_libre == 0)
        ocupadas.wait(guarda);

    const int valor_extraido = vec[primera_libre - 1];
    primera_libre --;

    //señalamos a la hebra productora en caso de que esta esté bloqueada
    libres.notify_one();

    //devolvemos el valor
    return valor_extraido;


}

/**************************************************************************/

void funcion_hebra_productora(ProdConsSC * monitor , int ID_productor) {

    for(int i = 0 ; i < num_items / num_productores ; ++i){
        int valor = producir_dato(ID_productor);
        monitor->insertar(valor);
    }

}

void funcion_hebra_consumidora(ProdConsSC * monitor) {

    for(int i = 0 ; i < num_items / num_consumidores ; ++i){
        int valor_consumido;
        valor_consumido = monitor->extraer();
        consumir_dato(valor_consumido);
    }

}


/**************************************************************************/


int main(){


    ProdConsSC monitor(num_productores , num_consumidores);

    for(int i = 0 ; i < num_productores ; ++i)
        productor[i] = thread(funcion_hebra_productora , &monitor , i);
    for(int j = 0 ; j < num_consumidores ; ++j)
        consumidor[j] = thread(funcion_hebra_consumidora , &monitor);

    for(int i = 0 ; i < num_productores ; ++i)
        productor[i].join();

    for(int j = 0 ; j < num_consumidores ; ++j)
        consumidor[j].join();



    test_contadores();

    return 0;
}