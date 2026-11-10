#include "Particula.h"

Particula::Particula(int id, double x, double y, double vx, double vy, double masa, double radio)
    : id(id), x(x), y(y), vx(vx), vy(vy), masa(masa), radio(radio), activa(true) {}

void Particula::mover(double dt) {
    if (activa) {
        x += vx * dt;
        y += vy * dt;
    }
}