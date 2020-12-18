#include <iostream>
#include <random>  // para 'aleatorio'
#include <thread>  // this_thread::sleep_for
#include <chrono>  // chrono::duration, chrono::milliseconds
#include <cstring>  // strlen
#include <mpi.h>

using namespace std;


const int id_emisor = 0 , 
          id_receptor = 1 , 
          num_procesos_esperado = 2;

int main(int argc , char *argv[]){

    int id_proceso , num_procesos;

    MPI_Init(&argc , &argv);

    MPI_Comm_rank(MPI_COMM_WORLD , &id_proceso); //obtenemos el id
    MPI_Comm_size(MPI_COMM_WORLD , &num_procesos); //obtenemos el tam.ç

    if(num_procesos == num_procesos_esperado){ //OK

        if(id_proceso == id_emisor){
            int valor_enviado ;
            cout<<"Introduce valor:"; cin>>valor_enviado;
            MPI_Send(&valor_enviado , 1 , MPI_INT , id_receptor , 
            id_emisor , MPI_COMM_WORLD);
            cout<<"Proceso emisor envia "<<valor_enviado;
        }else{
            int valor_recibido;
            MPI_Status estado; //var. para almacenar
                               // el estado de la recepcion
            MPI_Recv(&valor_recibido , 1 , MPI_INT , MPI_ANY_SOURCE , 
            MPI_ANY_TAG , MPI_COMM_WORLD , &estado);
            cout<<"Proceso receptor a recibido "<<valor_recibido<<endl;
        }

    }else{
        cout<<"ERROR: Num procesos esperado : "<<num_procesos_esperado
        <<endl;

    }


    MPI_Finalize();
    return 0;
}