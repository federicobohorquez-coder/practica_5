#ifndef REPRESENTANTE_H
#define REPRESENTANTE_H

#include <string>
#include "Particula.h"

/*
 * Representante: el "Rival". Esta dentro de la infraestructura.
 * Si un proyectil del jugador contrario lo toca, ese jugador gana (3.5).
 */
class Representante {
public:
    int propietario;
    double x, y, ancho, alto;   // esquina de menor x y menor y
    bool vivo;

    Representante(int propietario = 0, double x = 0, double y = 0,
                  double ancho = 0, double alto = 0)
        : propietario(propietario), x(x), y(y), ancho(ancho), alto(alto), vivo(true) {}

    // true si el circulo de la particula toca el rectangulo del representante
    bool tocadoPor(const Particula& p) const;
};

#endif // REPRESENTANTE_H