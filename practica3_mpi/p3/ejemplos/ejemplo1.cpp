#include <mpi.h>
#include <iostream>

using namespace std;

const int id_emisor = 0,
	id_receptor = 1,
	num_procesos_esperado = 2;

int main(int argc , char *argv[]){

	int id_propio , num_procesos;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&num_procesos);
	MPI_Comm_rank(MPI_COMM_WORLD,&id_propio);


	if(num_procesos == num_procesos_esperado){

		if(id_propio == id_emisor){

			int valor_enviado = 200; //buf_emi
			MPI_Send(&valor_enviado,1,MPI_INT,id_receptor,0,MPI_COMM_WORLD);
			cout<<"Emisor envía "<< valor_enviado<<endl;
		}else{ //Es el proceso receptor
			
			int valor_recibido; //buf_rec
			MPI_Status estado; //Estado de la recepcion
			MPI_Recv(&valor_recibido,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&estado);
			cout<<"El receptor recibe "<<valor_recibido<<endl;

		}

	}else{

		cout<<"ERROR: Esperados "<<num_procesos_esperado<<" procesos , hay "<<num_procesos<<endl;	

	}


	MPI_Finalize();
	return 0;
}
