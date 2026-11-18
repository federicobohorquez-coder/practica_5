#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "Particula.h"

/*
  Proyectil: una Particula de la Parte 1 que ademas tiene dueno y gravedad.
 Hereda de Particula, asi que Caja::rebotar(...) (paredes elasticas, punto 3.2)
 y Bloque::colisiona(...) (punto 3.4) funcionan con el sin cambios.

 Movimiento (ecuaciones (1) y (2) del enunciado, con y hacia abajo):
      x = x0 + vx * t
      y = y0 + vy * t + 1/2 * g * t^2        (vy0 < 0 al disparar hacia arriba)
 */
class Proyectil : public Particula {
private:
    int propietario;   // jugador que lo disparo (0 o 1)

public:
    // Dispara desde (x, y) con un angulo (grados, sobre la horizontal) y una rapidez.
    // direccion = +1 dispara hacia la derecha, -1 hacia la izquierda.
    Proyectil(int propietario = 0, double x = 0, double y = 0,
              double anguloGrados = 0, double velocidad = 0, int direccion = 1,
              double masa = 1.0, double radio = 1.0);

    int getPropietario() const { return propietario; }

    // Un paso de tiempo con gravedad (formulas exactas para aceleracion constante).
    void avanzar(double dt, double gravedad);

    double rapidez() const;
    double momentoLineal() const { return getMasa() * rapidez(); }   // |p| = m * |v|
};

#endif