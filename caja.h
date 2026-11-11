#ifndef CAJA_H
#define CAJA_H

#include "Particula.h"

class Caja {
public:
    double xmin, xmax, ymin, ymax;

    Caja(double xmin = 0, double xmax = 100, double ymin = 0, double ymax = 60)
        : xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax) {}

    bool rebotar(Particula& p) {
        bool reboto = false;
        if (p.getX() - p.getRadio() <= xmin || p.getX() + p.getRadio() >= xmax) {
            p.setVx(-p.getVx());
            reboto = true;
        }
        if (p.getY() - p.getRadio() <= ymin || p.getY() + p.getRadio() >= ymax) {
            p.setVy(-p.getVy());
            reboto = true;
        }
        return reboto;
    }
};

#endif