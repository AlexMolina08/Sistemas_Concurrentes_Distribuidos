// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Seminario 2. Introducción a los monitores en C++11.
//
// Compilar con : g++ -pthread -std=c++11 -o bin/barrera1_sc src/barrera1_sc.cpp 
//
// EJEMPLO MONITOR DE BARRERA PARCIAL:
//  Diseñar un monitor con UNA ÚNICA COLA CONDICIÓN que cumpla los siguientes
//  requerimientos:
//    
//      1) N procesos usan el monitor , ejecutan un bucle donde en cada iteración
//         realizan un retraso y ejecutan el metodo cita del monitor
//      2) Se espera que lleguen m hebras ( m < n ) para continuar la ejecución
//      3) Por tanto , cada conjunto de m hebras se esperan entre ellas
//      4) La última hebra en 
//
// -----------------------------------------------------------------------------

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include <condition_variable>

using namespace std;

const int NUM_HEBRAS = 100;
const int M = 10;

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

//MONITOR DE TIPO SC (Señalar y continuar)

class MonitorBarrera2 {

    private:
    int c ,          //contador de hebras que entran en cita
        m ; //numero de hebras que tienen que esperar antes de salir

    mutex cerrojo; 

    condition_variable cola; //COLA DE HEBRAS QUE ESPERAN (c == m)

    public: 
    MonitorBarrera2(int num_hebras);
    void cita(int num_hebra);


};

MonitorBarrera2::MonitorBarrera2(int max_hebras){
    c = 0;
    m = max_hebras;
}

void MonitorBarrera2::cita(int num_hebra){
    unique_lock<mutex> guarda(cerrojo); //ganamos e.m
    c ++; //incrementamos el numero de hebras que han llegado a cita

    const int orden = c;
    cout<< "Llega la hebra "<<num_hebra<<"("<<orden<<")"<<endl;

    if(c < m ){
        cola.wait(guarda); //si no es la ultima hebra , la bloqueamos
    }else{
        //si es la ultima hebra del subconjunto , desbloquea a las otras 
        // m hebras y continua su ejecución
        for(int i = 0 ; i < m - 1 ; ++i)
            cola.notify_one();
        
        c = 0; //reiniciamos el contador de hebras que han llegado a cita
    }

    //la primera hebra en ejecutar este mensaje será la última entrar en cita ,
    //ya que es la que ha ejecutado notify_one
    cout<< "\tSale la hebra "<<num_hebra<<"("<<orden<<")"<<endl;

}

void funcion_hebra(MonitorBarrera2 * monitor , int num_hebra){
    while(true){
        const int ms = aleatorio<10,100>();
        this_thread::sleep_for(chrono::milliseconds(ms));
        monitor -> cita(num_hebra);
    }
}

int main(){

    MonitorBarrera2 monitor_barrera(M);

    thread hebra[NUM_HEBRAS];

    for(int i = 0 ; i < NUM_HEBRAS ; ++i)
        hebra[i] = thread(funcion_hebra , &monitor_barrera , i );
    
    for(int i = 0 ; i < NUM_HEBRAS ; ++i)
        hebra[i].join();

    return 0;
}