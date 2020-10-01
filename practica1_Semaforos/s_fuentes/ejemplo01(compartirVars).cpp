// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplos (del 1 - 4) (ejemplo1.cpp)
// 
// Se crea funcion de factorial y función que lo asigna a var.global y otra función que lo 
// asigna por referencia para obtenerlo desde la hebra principal
//
// Septiembre 2020
//
// -----------------------------------------------------------------------------



#include <iostream>
#include <thread>     // declaraciones del tipo {\bf std::thread}
using namespace std ; // permite acortar la notación

//VARIABLES BLOBALES
long global_1 , global_2;

//Hebras
thread hebra_1, hebra_2 , hebra_3 , hebra_4; 

/*
  *Funcion que devuelve el factorial de un entero
*/
long factorial(int n){
  return (n<=0) ?  1 : (n * factorial(n-1) ) ;
}

/*
  * Funcion que ejecutará la hebra 1 , asigna el factorial a una var.global
*/
void funcion_hebra_1_global(int num1){
  global_1 = factorial(num1);
}

/*
  * Funcion que ejecutará la hebra 2 , asigna el factorial a otra var.global
*/
void funcion_hebra_2_global(int num2){
  global_2 = factorial(num2);
}

/*
  * Funcion que asigna al parametro por referencia el factorial 
*/

void funcion_hebra_referencia(int num , long &result){
  result = factorial(num);
}

int main()
{
  long resultado , resultado2; //Aqui se almacenan los resultados de la función por referencia

  hebra_1 = thread(funcion_hebra_1_global , 5);
  hebra_2 = thread(funcion_hebra_2_global , 6);
  hebra_3 = thread(funcion_hebra_referencia , 7 , ref(resultado));
  hebra_4 = thread(funcion_hebra_referencia , 8 , ref(resultado2));

  hebra_1.join() ; hebra_2.join(); hebra_3.join() ; hebra_4.join();

  cout<<"Factorial de 5 "<<global_1<<endl
  <<"Factorial de 6 "<<global_2<<endl
  <<"Factorial de 7 "<<resultado<<endl
  <<"Factorial de 8 "<<resultado2<<endl;
  

  
  return 0;
}
