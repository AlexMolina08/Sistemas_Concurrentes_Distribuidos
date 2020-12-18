// -----------------------------------------------------------------------------
// ALEJANDRO MOLINA CRIADO
// 17 / 12 / 2020
// -----------------------------------------------------------------------------

#include <iostream>
#include <random>  // para 'aleatorio'
#include <thread>  // this_thread::sleep_for
#include <chrono>  // chrono::duration, chrono::milliseconds
#include <cstring>  // strlen
#include <mpi.h>

const int num_procesos = 2;

int main(int argc , char* argv[]){

    MPI_Init(&argc , &argv);


    MPI_Finalize();


    return 0;
}