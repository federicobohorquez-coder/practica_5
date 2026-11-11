#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "Particula.h"

class Obstaculo {
public:
    int id;
    double x, y, lado;

    Obstaculo(int id, double x, double y, double lado)
        : id(id), x(x), y(y), lado(lado) {}

    bool colisiona(Particula& p, double e) {
        (void)p; (void)e;
        return false; // Persona A implementará la lógica real
    }
};

#endif