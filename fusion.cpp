#include "Fusion.h"
#include <cmath>

namespace Fusion {

//dos partículas chocan si la distancia entre sus centros es menor o igual que la suma de sus radios
bool chocan(const Particula& a, const Particula& b) {
    if (!a.isActiva() || !b.isActiva()) return false; // Particula ya fusionada
    const double dx = a.getX() - b.getX();
    const double dy = a.getY() - b.getY();
    const double s  = a.getRadio() + b.getRadio();
    return dx * dx + dy * dy <= s * s;     // Detecta el contacto comparando distancias
}

void fusionar(Particula& a, Particula& b) {
    // Se calcula todo con temporales antes de modificar 'a'
    const double m1 = a.getMasa(), m2 = b.getMasa();
    const double M  = m1 + m2;
//Calcula la masa total
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