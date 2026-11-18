#include "Proyectil.h"
#include <cmath>

static const double PI = 3.14159265358979323846;

Proyectil::Proyectil(int propietario, double x, double y, double anguloGrados,
                     double velocidad, int direccion, double masa, double radio)
    : Particula(0, x, y,
                direccion * velocidad * std::cos(anguloGrados * PI / 180.0),   // vx = dir * v * cos(a)
                -velocidad * std::sin(anguloGrados * PI / 180.0),              // vy = -v * sen(a)  (hacia arriba)
                masa, radio),
    propietario(propietario) {}

void Proyectil::avanzar(double dt, double gravedad) {
    if (!isActiva()) return;
    setX(getX() + getVx() * dt);                                   // x = x0 + vx*t
    setY(getY() + getVy() * dt + 0.5 * gravedad * dt * dt);        // y = y0 + vy*t + 1/2 g t^2
    setVy(getVy() + gravedad * dt);                                // vy = vy0 + g*t
}

double Proyectil::rapidez() const {
    return std::sqrt(getVx() * getVx() + getVy() * getVy());
}