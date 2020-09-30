// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplo 2
// Obtención de resultados mediante 'return' y la llamada a 'async'
//
// Historial:
// Creado en Abril de 2017
// -----------------------------------------------------------------------------

#include <iostream>
#include <future>     // declaracion de {\bf std::thread}, {\bf std::async}, {\bf std::future}
using namespace std ; // permite acortar la notación (abc en lugar de std::abc)

// declaración de la función {\bf factorial} (parámetro {\bf int}, resultado {\bf long})
long factorial( int n ) { return n > 0 ? n*factorial(n-1) : 1 ; }



int main()
{
  
  future<long> futuro1 = async(launch::async , factorial , 5); 
  future<long> futuro2 = async(launch::async , factorial , 6);

  cout<<"Factorial de 5 "<<futuro1.get()<<endl
  <<"Factorial de 6 "<<futuro2.get()<<endl;
}
