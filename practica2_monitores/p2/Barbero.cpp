#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"
#include "HoareMonitor.h"

using namespace std;
using namespace HM;

mutex mtx;
const int n = 4; // Numero de clientes

template<int min, int max> int aleatorio (){
  static default_random_engine generador((random_device())());
  static uniform_int_distribution<int> distribucion_uniforme(min, max);
  return distribucion_uniforme(generador);
}

void esperarFueraBarberia (int num_cliente){
    // calcular milisegundos aleatorios de duración de la acción de esperar fuera)
    chrono::milliseconds duracion_esperar_fuera(aleatorio<20,200>());

    // informa de que comienza a esperar fuera
    mtx.lock();
    cout << "Cliente " << num_cliente << "  :" << " empieza a esperar fuera de la barbería (" << duracion_esperar_fuera.count() << " milisegundos)" << endl;
    mtx.unlock();

    // espera bloqueada un tiempo igual a 'duracion_esperar_fuera' milisegundos
    this_thread::sleep_for(duracion_esperar_fuera);

    // informa de que ha terminado de esperar fuera
    mtx.lock();
    cout << "Cliente " << num_cliente << "  : termina de esperar fuera." << endl;
    mtx.unlock();
}

void cortarPeloACliente (){
    // calcular milisegundos aleatorios de duración de la acción de cortar)
    chrono::milliseconds duracion_cortar(aleatorio<20,200>());

    // informa de que comienza a cortar
    mtx.lock();
    cout << "Cortando pelo al cliente durante " << duracion_cortar.count() << " milisegundos" << endl;
    mtx.unlock();

    // espera bloqueada un tiempo igual a 'duracion_cortar' milisegundos
    this_thread::sleep_for(duracion_cortar);

    // informa de que ha terminado de cortar
    mtx.lock();
    cout << "He terminado de cortar el pelo, comienza espera de nuevo cliente" << endl;
    mtx.unlock();
}

class Barberia : public HoareMonitor{
private:
    CondVar sala_espera, silla, barbero;

public:
    Barberia ();
    void cortarPelo (int num_cliente);
    void siguienteCliente ();
    void finCliente ();
};

Barberia::Barberia (){
    sala_espera = newCondVar ();
    silla = newCondVar ();
    barbero = newCondVar ();
}

void Barberia::cortarPelo (int num_cliente){
    cout << "El cliente " << num_cliente << " ha entrado en la barberia\n";

    if (!barbero.empty()){ // Si el barbero esta durmiendo
        mtx.lock();
        cout << "El cliente " << num_cliente << " encuentra la barberia vacia\n";
        mtx.unlock();

        barbero.signal(); // Despierta al barbero
    }
    else{ // Si el barbero esta cortando el pelo
        mtx.lock();
        cout << "El cliente " << num_cliente << " se sienta en la sala de espera\n";
        mtx.unlock();

        sala_espera.wait(); // Espera a que lo llamen

        mtx.lock();
        cout << "El cliente " << num_cliente << " ha acabado de esperar\n";
        mtx.unlock();
    }

    cout << "El cliente " << num_cliente << " se ha sentado en la silla\n";

    silla.wait(); // Se sienta en la silla de pelar

    cout << "El cliente " << num_cliente << " ha terminado de cortarse el pelo\n";
}

void Barberia::siguienteCliente (){
    if (sala_espera.empty() and silla.empty()){ // no hay nadie en la barberia
        mtx.lock();
        cout << "No hay nadie en la barberia, el barbero se duerme\n";
        mtx.unlock();

        barbero.wait(); // El barbero se duerme

        mtx.lock();
        cout << "El barbero se despierta\n";
        mtx.unlock();
    }
    else if (silla.empty()){ // hay alguien en la sala de espera y no en la silla
        mtx.lock();
        cout << "Silla vacia, que entre el siguiente cliente\n";
        mtx.unlock();

        sala_espera.signal(); // Llama a un cliente
    }
}

void Barberia::finCliente (){
    mtx.lock();
    cout << "El cliente ha terminado. Silla libre\n";
    mtx.unlock();

    silla.signal(); // Vacia la silla
}

void funcion_hebra_barbero (MRef<Barberia> barberia){
    while (true){
        barberia->siguienteCliente();
        cortarPeloACliente();
        barberia->finCliente();
    }
}

void funcion_hebra_cliente (MRef<Barberia> barberia, int num_cliente){
    while (true){
        barberia->cortarPelo(num_cliente);
        esperarFueraBarberia(num_cliente);
    }
}

int main (){
    cout << "-------------------------------------------------------------------------------" << endl
         << "Problema del barbero durmiente." << endl
         << "-------------------------------------------------------------------------------" << endl
         << flush;

    MRef<Barberia> barberia = Create<Barberia>();
    thread h_barbero, h_clientes[n];

    h_barbero = thread(funcion_hebra_barbero, barberia);

    for (int i = 0; i < n; i++)
        h_clientes[i] = thread(funcion_hebra_cliente, barberia, i);

    for (int i = 0; i < n; i++)
        h_clientes[i].join();

    h_barbero.join();
}
