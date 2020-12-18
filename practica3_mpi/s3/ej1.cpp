#include <iostream>
#include <random>  // para 'aleatorio'
#include <thread>  // this_thread::sleep_for
#include <chrono>  // chrono::duration, chrono::milliseconds
#include <cstring>  // strlen
#include <mpi.h>

using namespace std;

int num_procesos , id;

int main(int argc , char *argv[]){
  MPI_Init(&argc , &argv);

  MPI_Comm_size(MPI_COMM_WORLD , &num_procesos);
  MPI_Comm_rank(MPI_COMM_WORLD , &id);

  cout<<"hola desde el proceso "<<id<<" de "<<num_procesos<<endl;

  MPI_Finalize();
  return 0;
}