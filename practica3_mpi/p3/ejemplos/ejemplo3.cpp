#include <mpi.h>
#include <iostream>

using namespace std;

int main(int argc , char* argv[]){
	MPI_Init(&argc,&argv);
	
	int num_procesos_actual , id_propio;

	MPI_Comm_size(MPI_COMM_WORLD,&num_procesos_actual);		
	MPI_Comm_rank(MPI_COMM_WORLD,&id_propio);
	
	if(num_procesos_actual % 2 == 0){

		int valor_enviado = id_propio * 2,
			valor_recibido,
			id_vecino;

			MPI_Status estado;
	
		if(id_propio % 2 == 0){ //Si el ID es par, envía y luego recibe
	
			id_vecino = id_propio + 1;
			MPI_Ssend(&valor_enviado , 1 , MPI_INT , id_vecino , 0  , MPI_COMM_WORLD);
			MPI_Recv(&valor_recibido , 1 , MPI_INT , id_vecino , 0 , MPI_COMM_WORLD , &estado);


		}else{ //Si el ID es impar , recibe y luego envía

			id_vecino = id_propio - 1;
			MPI_Recv(&valor_recibido , 1 , MPI_INT , id_vecino , 0 , MPI_COMM_WORLD , &estado);
			MPI_Ssend(&valor_enviado , 1 , MPI_INT , id_vecino , 0  , MPI_COMM_WORLD);
		}

		cout<<"P"<<id_propio<<" envía "<<valor_enviado<<" || recibe "<<valor_recibido<<endl;

	}else{

		cerr<<"ERROR: nº procesos debe ser par\n";	

	}	


	MPI_Finalize();
	return 0;
}






//
