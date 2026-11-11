#include "Fusion.h"
#include <cmath>

namespace Fusion {

bool chocan(const Particula& a, const Particula& b) {
    if (!a.isActiva() || !b.isActiva()) return false;
    const double dx = a.getX() - b.getX();
    const double dy = a.getY() - b.getY();
    const double s  = a.getRadio() + b.getRadio();
    return dx * dx + dy * dy <= s * s;     // sin raiz cuadrada
}

void fusionar(Particula& a, Particula& b) {
    // Se calcula todo con temporales antes de modificar 'a'
    const double m1 = a.getMasa(), m2 = b.getMasa();
    const double M  = m1 + m2;

    const double vx = (m1 * a.getVx() + m2 * b.getVx()) / M;   // momento
    const double vy = (m1 * a.getVy() + m2 * b.getVy()) / M;
    const double x  = (m1 * a.getX()  + m2 * b.getX())  / M;   // centro de masa
    const double y  = (m1 * a.getY()  + m2 * b.getY())  / M;
    const double r  = std::sqrt(a.getRadio() * a.getRadio() +
                               b.getRadio() * b.getRadio());  // area conservada

    a.setMasa(M);
    a.setVx(vx);  a.setVy(vy);
    a.setX(x);    a.setY(y);
    a.setRadio(r);
    b.setActiva(false);
}

}