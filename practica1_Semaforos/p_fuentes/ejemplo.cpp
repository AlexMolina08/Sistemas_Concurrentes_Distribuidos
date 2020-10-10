#include <random>
#include <iostream>
#include <chrono>
#include <thread>
#include "Semaphore.h"

using namespace std;
using namespace std::chrono;
using namespace SEM ;


semaphore puede_leer = 0;
int compartida ;





/*
    generar un entero aleatorio a partir de 2 constantes conocidas
    en tiempo de compilación
*/
template<int min, int max > int aleatorio()
{
    static default_random_engine generador( (random_device())() );
    static uniform_int_distribution<int> distribucion_uniforme( min, max );
    return distribucion_uniforme( generador );
}

void producir(){
    int a ;
    a = aleatorio<100,200>();

}

void productor(){
    int a;
    
}

int main(){


    return 0;
}