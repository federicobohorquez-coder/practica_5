#ifndef FUSION_H
#define FUSION_H

#include "Particula.h"
namespace Fusion {
// true si la distancia entre centros <= r1 + r2 (y ambas estan activas)
bool chocan(const Particula& a, const Particula& b);

// El cuerpo combinado queda en 'a'; 'b' se marca como inactiva.
void fusionar(Particula& a, Particula& b);
}

#endif