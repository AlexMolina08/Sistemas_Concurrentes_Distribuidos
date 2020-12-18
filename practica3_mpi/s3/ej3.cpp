#include <iostream>
#include <random>  // para 'aleatorio'
#include <thread>  // this_thread::sleep_for
#include <chrono>  // chrono::duration, chrono::milliseconds
#include <cstring>  // strlen
#include <mpi.h>

using namespace std;

const int num_min_procesos = 2;


int leer_dato(){
    int dato;
    cout<<"Introduce un entero: ";
    cin>>dato;

    return dato;
}

int main(int argc , char *argv[]){

    int num_procesos , 
        id;

    MPI_Init(&argc , &argv);

    MPI_Comm_size(MPI_COMM_WORLD , &num_procesos);
    MPI_Comm_rank(MPI_COMM_WORLD , &id);

    if (num_min_procesos <= num_procesos){ //OK.

        const int anterior = id - 1 , 
                  siguiente = id + 1;
        MPI_Status estado;
        int dato;

        //---- ENTRADA DEL VALOR EN EL PROCESO

        if (id == 0) //PRIMER PROCESO
            dato = leer_dato();
        else        //RESTO DE PROCESOS
            MPI_Recv(&dato,1,MPI_INT,anterior,0,MPI_COMM_WORLD,&estado);
        //-------------------------------------

        cout<<"Proceso "<<id<<" recibe "<<dato<<endl;

        if(siguiente < num_procesos){ //Si no es el ultimo
            MPI_Send(&dato,1,MPI_INT,siguiente,0,MPI_COMM_WORLD);
            cout<<"\tProceso "<<id<<" envia "<<dato<<endl;

        }


    }else{
        cout<<"ERROR: Num. procesos minimos 2\n";
    }


    MPI_Finalize();
    return 0;

}