#include "Simulacion.h"
#include "LectorSimulacion.h"
#include <iostream>

int main() {
    std::cout << "=== INICIANDO PROGRAMA ===" << std::endl;

    Simulacion sim(0.01, 20.0, 0.7, "simulacion_salida.txt");

    sim.setCaja(Caja(0.0, 100.0, 0.0, 60.0));
    sim.agregarObstaculo(Obstaculo(0, 20.0, 10.0, 8.0));
    sim.agregarObstaculo(Obstaculo(1, 70.0, 10.0, 8.0));
    sim.agregarObstaculo(Obstaculo(2, 20.0, 40.0, 8.0));
    sim.agregarObstaculo(Obstaculo(3, 70.0, 40.0, 8.0));

    sim.agregarParticula(Particula(0, 10.0, 30.0, 15.0, 5.0, 1.0, 2.0));
    sim.agregarParticula(Particula(1, 90.0, 30.0, -12.0, -4.0, 1.5, 2.2));
    sim.agregarParticula(Particula(2, 50.0, 50.0, 3.0, -10.0, 2.0, 2.5));
    sim.agregarParticula(Particula(3, 50.0, 10.0, -5.0, 8.0, 1.2, 2.1));

    std::cout << "Ejecutando simulacion..." << std::endl;
    sim.ejecutar();
    std::cout << "Simulacion ejecutada exitosamente." << std::endl;

    LectorSimulacion lector;
    if (lector.cargarArchivo("simulacion_salida.txt")) {
        std::cout << "EXITO: Frames cargados: " << lector.getFrames().size() << std::endl;
    } else {
        std::cout << "ERROR: No se pudo abrir 'simulacion_salida.txt'" << std::endl;
    }

    return 0;
}