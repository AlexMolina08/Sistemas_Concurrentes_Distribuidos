// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplo 4 ( PARA ENTENDER std::chrono )
// Se ve que tarda mas , si calcular el factorial de 10 numeros (del 2 al 12)
// de forma secuencial o con 10 threads (tantos como factoriales a calcular)
//
// Historial:
// Creado en Abril de 2017
// -----------------------------------------------------------------------------

#include <iostream>
#include <chrono>
#include <thread>
using namespace std;
using namespace std::chrono;
const int n_hebras = 10;
thread hebras[n_hebras];

/*Funcion que devuelve el factorial de un entero*/
long factorial( int n ) { return n > 0 ? n*factorial(n-1) : 1 ; }

/*
Funcion para calcular factorial en una hebra
*/
void factorial_hebra(int num){
    long result = factorial(num);
    cout<<"factorial de "<<num<<" : "<<result<<endl<<flush;
}

int main(){

    int factorial_maximo = 12; //Factorial maximo a calcular

    //puntos de inicio y de fin del reloj
    time_point<steady_clock> start_point;
    time_point<steady_clock> end_point;
    
    //duracion
    duration<float,micro> duration_secuencial;
    duration<float,micro> duration_paralelo;
    

    /**************************
    Ejecución secuencial
    ***************************/

    cout<<"\nEJECUCION SECUENCIAL\n";
    start_point = steady_clock::now(); //inicio del reloj
    for(int i = 2 ; i < factorial_maximo ; ++i){
        cout<<"factorial de "<<i<<" : "<<factorial(i)<<endl;
    }
    end_point = steady_clock::now(); // fin del reloj
    duration_secuencial = end_point - start_point;
    cout<<"\n\t#Tiempo: "<<duration_secuencial.count()<<"ms\n\n";

    /**************************
        Ejecución paralela
    ***************************/
    
    int fact = 2; //contador del factorial

    cout<<"\nEJECUCION PARALELA\n";

    start_point = steady_clock::now();

    //lanzamos las hebras
    for(int i = 0 ; i < n_hebras ; ++i){
        hebras[i] = thread(factorial_hebra , fact);
        fact++;
    }

    for(int j = 0 ; j < n_hebras ; ++j){
        hebras[j].join();
    }

    end_point = steady_clock::now();
    duration_paralelo = end_point - start_point;
    cout<<"\n\t#Tiempo: "<<duration_paralelo.count()<<"ms\n\n";



    cout<<endl<<endl<<factorial(400)<<endl;

    return 0;
}