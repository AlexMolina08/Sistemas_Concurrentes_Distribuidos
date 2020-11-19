
#include <iostream>
#include <iomanip>
#include <cassert>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include "HoareMonitor.h"

using namespace std ; 
using namespace HM ;
 

const int num_escritoras = 2;
const int num_lectoras = 3;

 
 template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

class Lec_Esc : public HoareMonitor
{
 private:
 int  n_lec;	//numero de lectores leyendo
 bool escrib;	//es true si hay algun escritor escribiendo
 int lectores_permitidos; // Número de lectores permitidos si hay escritor encolado
 const int MAX_PERMITIDOS=3;
 CondVar
 	lectura,	
 	escritura, 	
	revision_entrada , 
	revision_salida;

 public:
 	Lec_Esc();	// constructor y métodos públicos
 	void ini_lectura();		//invocadas por lectores		
 	void fin_lectura();
 	void ini_escritura();		//invocadas por escritores
 	void fin_escritura();

	void ini_revision();
	void fin_revision();
 	
 };
 
 Lec_Esc::Lec_Esc()
 {
 	n_lec = 0;
 	escrib = false;
        lectores_permitidos=MAX_PERMITIDOS; //Número permitido máximo
 	lectura = newCondVar();
 	escritura = newCondVar();
	revision_entrada = newCondVar();
	revision_salida = newCondVar();
 }
 
 void Lec_Esc::ini_lectura()
 {
 	if (escrib || (!escritura.empty() && lectores_permitidos==0)) 
            lectura.wait();
        n_lec++;
        if (!escritura.empty()) lectores_permitidos--;
        if (lectores_permitidos>0)
 	  lectura.signal();
 	
 }
 
 void Lec_Esc::fin_lectura()
 {
 	n_lec--;
 	if(n_lec == 0)
            escritura.signal();
                
 	
 }
 
 void Lec_Esc::ini_escritura()
 {
 	if(n_lec > 0 || escrib) 
            escritura.wait();

    revision_entrada.signal() ; //avisamos al revisor que ya puede entrar

	lectores_permitidos=MAX_PERMITIDOS;
    escrib = true;
 	
 }
 
 void Lec_Esc::fin_escritura()
 {
 	escrib = false;
 	if(!lectura.empty()){
 		lectura.signal();
 	} else
 	{
 		escritura.signal();
 	}
 }


void Lec_Esc::ini_revision(){
	if(escritura.get_nwt() < 1 )
		revision_entrada.wait();
	
	cout<<"Revision iniciada\n";


}

void Lec_Esc::fin_revision(){
	if(escritura.get_nwt() >= 1 )
		revision_salida.wait();

	cout<<"Revision terminada\n";
}

 
//----------------------------------------------------------------------
// función que ejecuta la hebra del lector

void Lector( MRef<Lec_Esc> monitor, int i)
{
	while(true)
	{
	   // calcular milisegundos aleatorios de duración de la acción de lector
	   chrono::milliseconds duracion_leer( aleatorio<10,15>() );
	   
	   // espera bloqueada un tiempo igual a ''duracion_leer' milisegundos
	   this_thread::sleep_for( duracion_leer );
	   
	   monitor->ini_lectura();
	   cout << "Lector " << i << " : empieza a leer" << endl; 
	   // calcular milisegundos aleatorios de duración de la acción de lector
	   chrono::milliseconds duracion_leer2( aleatorio<200,300>() );
	   // espera bloqueada un tiempo igual a ''duracion_leer' milisegundos
	   this_thread::sleep_for( duracion_leer2 );
	   
	 
           cout << "Lector " << i << " termina de leer" << endl;
	   monitor->fin_lectura();
		// calcular milisegundos aleatorios de duración de la acción de lector
	   chrono::milliseconds duracion_leer3( aleatorio<20,30>() );
	   // espera bloqueada un tiempo igual a ''duracion_leer' milisegundos
	   this_thread::sleep_for( duracion_leer3 );

	}
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del escritor
void  Escritor( MRef<Lec_Esc> monitor, int i )
{
   while(true)
	{
	   // calcular milisegundos aleatorios de duración de la acción de lector
	   chrono::milliseconds duracion_escribir( aleatorio<100,200>() );
	   
	   // espera bloqueada un tiempo igual a ''duracion_escribir' milisegundos
	   this_thread::sleep_for( duracion_escribir );
	   
	   monitor->ini_escritura();
	   cout << "............................................Escritor " << i << " empieza a escribir" << endl; 
	   chrono::milliseconds duracion_escribir2( aleatorio<100,200>() );
	   // espera bloqueada un tiempo igual a ''duracion_escribir' milisegundos
	   this_thread::sleep_for( duracion_escribir2 );
	   
	   cout << "............................................Escritor " << i << " termina de escribir, comienza espera de lectura" << endl; 
	   
	   monitor->fin_escritura();
	   chrono::milliseconds duracion_escribir3( aleatorio<100,200>() );
	   // espera bloqueada un tiempo igual a ''duracion_escribir' milisegundos
	   this_thread::sleep_for( duracion_escribir3 );

	}
}


// función que ejecuta la hebra del revisor
void  Revisor( MRef<Lec_Esc> monitor)
{
   while(true)
	{
	   monitor->ini_revision();
	   monitor->fin_revision();

	}
}


//----------------------------------------------------------------------


//----------------------------------------------------------------------

int main()
{

	MRef<Lec_Esc> monitor = Create<Lec_Esc>();
	
   cout << "--------------------------------------------------------" << endl
        << "Problema de los lectores-escritores." << endl
        << "--------------------------------------------------------" << endl
        << flush ;
        
        thread hebras_lector[num_lectoras],hebras_escritor[num_escritoras];
		thread revisora(Revisor , monitor);
        
        for(int i = 0; i < num_lectoras; i++){
        	hebras_lector[i] = thread(Lector, monitor, i);
        }
		for(int i = 0; i < num_escritoras; i++){
        	hebras_escritor[i] = thread(Escritor, monitor, i);
        }

		for(int i = 0; i < num_lectoras; i++){
        	hebras_lector[i].join();
        }
        
        for(int i = 0; i < num_escritoras; i++){
        	hebras_escritor[i].join();
        }



		

  
   cout << endl << endl;
}   
