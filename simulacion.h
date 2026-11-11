#ifndef SIMULACION_H
#define SIMULACION_H

#include <vector>
#include <string>
#include "Particula.h"
#include "Caja.h"
#include "Obstaculo.h"
#include "Fusion.h"

class Simulacion {
private:
    double dt;
    double tiempoTotal;
    double e;
    std::string nombreArchivo;

    Caja caja;
    std::vector<Obstaculo> obstaculos;
    std::vector<Particula> particulas;

public:
    Simulacion(double dt, double tiempoTotal, double e, const std::string& nombreArchivo);

    void setCaja(const Caja& c);
    void agregarObstaculo(const Obstaculo& obs);
    void agregarParticula(const Particula& p);

    void ejecutar();
};

#endif