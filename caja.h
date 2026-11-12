#ifndef CAJA_H
#define CAJA_H

#include "Particula.h"

class Caja {
public:
    double xmin, xmax, ymin, ymax;

    Caja(double xmin = 0, double xmax = 100, double ymin = 0, double ymax = 60)
        : xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax) {}

    // Devuelve true si hubo rebote en alguna pared (la Simulacion lo registra).
    bool rebotar(Particula& p) const;
};

#endif