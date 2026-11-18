#ifndef BLOQUE_H
#define BLOQUE_H

#include "Particula.h"
class Bloque {
public:
    int id;
    int propietario;          // jugador dueno del bloque (0 o 1)
    double x, y, ancho, alto;
    double resistencia;       // resistencia actual
    double resistenciaMax;    // resistencia inicial (para dibujar barras o colores)

    Bloque(int id, int propietario, double x, double y,
           double ancho, double alto, double resistencia)
        : id(id), propietario(propietario), x(x), y(y), ancho(ancho), alto(alto),
        resistencia(resistencia), resistenciaMax(resistencia) {}

    bool estaDestruido() const { return resistencia <= 0.0; }
    void recibirDano(double dano);
    bool colisiona(Particula& p, double e, double& velocidadImpacto) const;
};

#endif