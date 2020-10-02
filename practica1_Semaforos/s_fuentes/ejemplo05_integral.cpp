// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplo 5 (ejemplo9.cpp)
// Calculo concurrente de una integral midiendo tiempos de ejecución.
// Hay dos versiones del cálculo concurrente:
//    *)Con threads
//    *)Con la función async y futuros
//
// Historial:
// Creado en Abril de 2017
// Modificado en Septiembre de 2020
//
// Alejandro Molina Criado (Grupo B1)
//
// -----------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <chrono>  // incluye now, time_point, duration
#include <future>
#include <vector>
#include <cmath>

using namespace std ;
using namespace std::chrono;

const long m  = 1024l*1024l*1024l, // número de puntos (del orden de mil millones)
           n  = 4  ;               // número de hebras concurrentes (divisor de 'm')


double resultado_parcial[n]; //vector global que almacena los resultados parciales de cada hebra



// -----------------------------------------------------------------------------
// evalua la función $f$ a integrar ( f(x)=4/(1+x^2) )
double f( double x )
{
  return 4.0/(1.0+x*x) ;
}
// -----------------------------------------------------------------------------
// calcula la integral de forma secuencial, devuelve resultado
double calcular_integral_secuencial()
{
   double suma = 0.0 ;                        // inicializar suma
   for( long j = 0 ; j < m ; j++ )            // para cada j entre 0 y m-1:
   {  const double xj = double(j+0.5)/m ;     //      calcular x_j
      suma += f( xj );                        //      añadir f(x_j) a la suma actual
   }
   return suma/m ;                            // devolver valor promedio de $f$
}


/******************************************************************************

      VERSIÓN UTILIZANDO thread

******************************************************************************/

// -----------------------------------------------------------------------------
// Funcion que ejecuta cada hebra: recibe $i$ ==índice de la hebra, ($0\leq i<n$)
// -----------------------------------------------------------------------------

void parcial( long i )
{
   double suma_parcial = 0; 

   //Calculamos los valores que toma la funcion en los puntos correspondientes
   //de forma entrelazada (va saltando de n en n)
   for(unsigned j = i; j < m; j+=n)
      suma_parcial += f( (j+0.5) / m ); 

   resultado_parcial[i] = suma_parcial;
   
}

// -----------------------------------------------------------------------------
// Devuelve el resultado de la integral
// -----------------------------------------------------------------------------

double calcular_integral_concurrente( )
{
   double resultado = 0;
   thread hebras[n]; //vector local de hebras

   //lanzamos todas las hebras y esperamos a que terminen
   for(unsigned i = 0 ; i < n ; ++i)
      hebras[i] = thread(parcial , i);
   for(unsigned i = 0 ; i < n ; ++i)
      hebras[i].join();

   //Una vez finalizadas todas las hebras , sumamos todos los resultados parciales
   for(int i = 0 ; i<n ; ++i){
      resultado += resultado_parcial[i];
   }

   resultado /= m;  //Obtenemos valor promedio  

   return resultado ;
}


/******************************************************************************

      VERSIÓN UTILIZANDO async

******************************************************************************/

double parcial_f(long i){
   double parcial;
   double suma_parcial = 0;

   //Calculamos los valores que toma la funcion en los puntos correspondientes
   //de forma entrelazada (va saltando de n en n)
   for(unsigned j = i ; j < m ; j+=n)
      parcial += f( (j+0.5) / m ); 

   return parcial;  
}

double calcular_integral_concurrente_f(){
   double resultado = 0;
   future<double> resultados[n];

   for(unsigned i = 0; i < n ; ++i){
      resultados[i] = async (launch::async , parcial_f , i);
   }
   
   for(int i = 0 ; i<n ; ++i)
      resultado += resultados[i].get();


   resultado /= m; //obtenemos el valor promedio

   return resultado;
}



int main()
{

  time_point<steady_clock> inicio_sec  = steady_clock::now() ;
  const double             result_sec  = calcular_integral_secuencial(  );
  time_point<steady_clock> fin_sec     = steady_clock::now() ;
  double x = sin(0.4567);

  time_point<steady_clock> inicio_conc = steady_clock::now() ;
  const double             result_conc = calcular_integral_concurrente();
  time_point<steady_clock> fin_conc    = steady_clock::now() ;

  time_point<steady_clock> inicio_conc_f = steady_clock::now() ;
  const double             result_conc_f = calcular_integral_concurrente_f();
  time_point<steady_clock> fin_conc_f    = steady_clock::now() ;


  duration<float,milli>    tiempo_sec  = fin_sec  - inicio_sec ,
                           tiempo_conc = fin_conc - inicio_conc ,
                           tiempo_conc_future = fin_conc_f - inicio_conc_f;

  

  const float              porc        = 100.0*tiempo_conc.count()/tiempo_sec.count() ;


  constexpr double pi = 3.14159265358979323846l ;

  cout << "\n*********************************************************\n"
       << "Entrega 1 . Cálculo Integral de forma concurrente\n"
       << "*********************************************************\n\n"
       << "Número de muestras (m):\t" << m << endl
       << "Número de hebras (n):\t" << n << endl
       << setprecision(18)
       << "Valor de PI:\t" << pi << endl
       << "Resultado secuencial:\t" << result_sec  << endl
       << "Resultado concurrente(thread version)    : " << result_conc << endl
       << "Resultado concurrente(async version)     : " << result_conc_f << endl
       << "Tiempo secuencial:\t" << tiempo_sec.count()  << " milisegundos. " << endl
       << "Tiempo concurrente(threads):\t" << tiempo_conc.count() << " milisegundos. " << endl
       << "Tiempo concurrente(async):\t" << tiempo_conc_future.count() << " milisegundos. " << endl
       << setprecision(4)
       << "Porcentaje t.conc/t.sec.:\t" << porc << "%" << endl;
}