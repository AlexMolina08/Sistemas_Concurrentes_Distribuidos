// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Seminario 2. Introducción a los monitores en C++11.
//
// Compilar con : g++ -pthread -std=c++11 -o bin/barrera1_sc src/barrera1_sc.cpp 
//
// EJEMPLO MONITOR DE BARRERA SIMPLE:
//  Diseñar un monitor con UNA ÚNICA COLA CONDICIÓN que cumpla los siguientes
//  requerimientos:
//    
//      1) N procesos usan el monitor , ejecutan un bucle donde en cada iteración
//         realizan un retraso y ejecutan el metodo cita del monitor
//      2) Todas las hebras se esperan entre ellas para ir a la siguiente iteración
//         (avanzan de forma síncrona)
//
// -----------------------------------------------------------------------------

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include <condition_variable>

using namespace std;

const int NUM_HEBRAS = 10;

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

//MONITOR DE TIPO SC (Señalar y continuar)

class MonitorBarrera1 {

    private:
    int c , //numero de hebras que han llegado a cita
        num_hebras ; //numero de hebras

    mutex cerrojo; 

    condition_variable cola; //COLA DE HEBRAS QUE ESPERAN (c == n)

    public: 
    MonitorBarrera1(int num_hebras);
    void cita(int num_hebra);



};

MonitorBarrera1::MonitorBarrera1(int n){
    c = 0;
    num_hebras = n;
}

void MonitorBarrera1::cita(int num_hebra){
    unique_lock<mutex> guarda(cerrojo); //ganamos e.m
    c ++; //incrementamos el numero de hebras que han llegado a cita

    const int orden = c;
    cout<< "Llega la hebra "<<num_hebra<<"("<<orden<<")"<<endl;

    if(c < num_hebras ){
        cola.wait(guarda); //si no es la ultima hebra , la bloqeamos
    }else{
        //si es la ultima hebra , despertamos al resto para que continuen la
        //iteración
        for(int i = 0 ; i < num_hebras ; ++i)
            cola.notify_one();
        
        c = 0; //reiniciamos el contador de hebras que han llegado a cita
    }

    //la primera hebra en ejecutar este mensaje será la última entrar en cita ,
    //ya que es la que ha ejecutado notify_one
    cout<< "\tSale la hebra "<<num_hebra<<"("<<orden<<")"<<endl;

}

void funcion_hebra(MonitorBarrera1 * monitor , int num_hebra){
    while(true){
        const int ms = aleatorio<10,100>();
        this_thread::sleep_for(chrono::milliseconds(ms));
        monitor -> cita(num_hebra);
    }
}

int main(){

    MonitorBarrera1 monitor_barrera(NUM_HEBRAS);

    thread hebra[NUM_HEBRAS];

    for(int i = 0 ; i < NUM_HEBRAS ; ++i)
        hebra[i] = thread(funcion_hebra , &monitor_barrera , i );
    
    for(int i = 0 ; i < NUM_HEBRAS ; ++i)
        hebra[i].join();

    return 0;
}