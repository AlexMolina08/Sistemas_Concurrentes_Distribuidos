#include <mpi.h>
#include <iostream>

using namespace std;

const int num_procesos_minimos  = 2;

int main(int argc, char *argv[]){
	
	int id_propio , num_procesos_actual;

	MPI_Init(&argc,&argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &num_procesos_actual);
	MPI_Comm_rank(MPI_COMM_WORLD, &id_propio);

	if(num_procesos_minimos <= num_procesos_actual){ //Si se han pasado 2 o mas procesos


	    int valor; //valor recibido / leído ó enviado  

		do{
		
		const int id_anterior = id_propio - 1,
				  id_siguiente = id_propio + 1;

		MPI_Status estado;	


		if(id_anterior < 0) //Si es el primer proceso , lee valor del teclado
			cin>>valor;		
		else //Si no es el primer proceso , recibe valor del anterior proceso
			MPI_Recv(&valor,1,MPI_INT,id_anterior,0,MPI_COMM_WORLD,&estado);

		cout<<"Proceso "<<id_propio<<" lee/recibe "<<valor<<endl;

		if(id_siguiente < num_procesos_actual) //Si no es el último proceso
			MPI_Send(&valor,1,MPI_INT,id_siguiente,0,MPI_COMM_WORLD);

		}while(valor >= 0);


	}else{

		cerr<<"ERROR: Minimos esperados "<<num_procesos_minimos<<" , Hay "<<num_procesos_actual<<endl;

	}

	MPI_Finalize();

	return 0;
}
