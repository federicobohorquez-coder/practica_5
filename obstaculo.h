#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "Particula.h"

class obstaculo {
public:
    int id;
    double x, y, lado;

    obstaculo(int id, double x, double y, double lado)
        : id(id), x(x), y(y), lado(lado) {}

    // Devuelve true si hubo colision (la Simulacion la registra como OBST).
    bool colisiona(Particula& p, double e) const;
};

#endif