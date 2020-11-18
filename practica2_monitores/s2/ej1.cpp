// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Seminario 2. Introducción a los monitores en C++11.
//
// Compilar con : g++ -pthread -std=c++11 -o bin/ej1 ej1.cpp 
//
// archivo: monitor_em.cpp
// Ejemplo de monitores en C++11 sin variables condición
// (solo con encapsulamiento y exclusión mutua)
//
//  -- MContador1 : sin e.m.
//  -- MContador2 : con e.m. mediante acceso directo a cerrojos
//  -- MContador3 : con e.m. mediante guardas de cerrojo
//
// Historial:
// Creado en Julio de 2017
// Modificado en Noviembre de 2020
// -----------------------------------------------------------------------------

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>

const int num_incrementos = 20;

using namespace std ;

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}


/*****************************************************************/
//          MONITOR SIN E.M
/*****************************************************************/


class MContador1 {
    private: 
    int contador;

    public:
    MContador1(int v_inicial);
    void incrementa();
    int leer_valor();
} ;


MContador1::MContador1(int v_inicial){
    contador = v_inicial;
}

void MContador1::incrementa(){
    contador ++;
}

int MContador1::leer_valor(){
    return contador;
}

/*****************************************************************/
//          MONITOR CON E.M (acceso directo a cerrojos)
/*****************************************************************/


class MContador2 {
    private: 
    int contador;
    mutex mtx;

    public:
    MContador2(int v_inicial);
    void incrementa();
    int leer_valor();
} ;


MContador2::MContador2(int v_inicial){
    contador = v_inicial;
}

void MContador2::incrementa(){
    mtx.lock();
    contador ++;
    mtx.unlock();
}

int MContador2::leer_valor(){
    int temp; 
    mtx.lock();
    temp = contador;
    //mtx.unlock();
    return temp;
}

/*****************************************************************/
//          MONITOR CON E.M (com guardas de cerrojo)
/*****************************************************************/

class MContador3 {

    private:
    int contador;
    mutex mtx; //CERROJO DEL MONITOR

    public:
    MContador3(int valor);
    void incrementa();
    int leer_valor();

};

MContador3::MContador3(int valor){
    contador = valor;
}

void MContador3::incrementa(){
    unique_lock<mutex> guarda(mtx); //GUARDA DE CERROJO (SE GANA E.M en el metodo)
    contador++;
}

int MContador3::leer_valor(){
    unique_lock<mutex> guarda(mtx); //GUARDA DE CERROJO (SE GANA E.M en el metodo)
    return contador;
}





/*****************************************************************/

void funcion_M1(MContador1* monitor){
    
    for(int i = 0 ; i < num_incrementos ; ++i)
        monitor->incrementa();
    
}

void test1(){
    cout<<"** TEST 1 **\n..ejecutando..\n";
    MContador1 monitor1(0);

    thread hebra1 (funcion_M1 , &monitor1);
    thread hebra2 (funcion_M1 , &monitor1);
    thread hebra3 (funcion_M1 , &monitor1);

    hebra1.join(); hebra2.join(); hebra3.join();

    cout << "Monitor contador (sin exclusión mutua):" << endl
         << endl
         << "  valor esperado == " << 3*num_incrementos << endl
         << "  valor obtenido == " << monitor1.leer_valor() << endl
         << endl;
}

void funcion_M2(MContador2* monitor){
    for(int i = 0 ; i < num_incrementos ; ++i)
        monitor->incrementa();
} 

void test2(){
    cout<<"** TEST 2 **\n..ejecutando..\n";
    MContador2 monitor2(0);
    thread hebra1 (funcion_M2 , &monitor2);
    thread hebra2 (funcion_M2 , &monitor2);
    hebra1.join(); hebra2.join();

    cout << "Monitor contador (con exclusión mutua):" << endl
         << endl
         << "  valor esperado == " << 2*num_incrementos << endl
         << "  valor obtenido == " << monitor2.leer_valor() << endl
         << endl;
}

void funcion_M3(MContador3* monitor){
    for(int i = 0 ; i < num_incrementos ; ++i)
        monitor->incrementa();
}

void test3(){
    cout<<"** TEST 3 **\n..ejecutando..\n";
    MContador3 monitor3(0);
    thread hebra1(funcion_M3 , &monitor3);
    thread hebra2(funcion_M3 , &monitor3);
    hebra1.join() ; hebra2.join();

    cout << "Monitor contador (con guarda de cerrojo):" << endl
         << endl
         << "  valor esperado == " << 2*num_incrementos << endl
         << "  valor obtenido == " << monitor3.leer_valor() << endl
         << endl;

}

int main(){

    test1();
    test2();
    test3();
    return 0;
}