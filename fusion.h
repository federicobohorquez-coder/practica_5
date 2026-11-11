#ifndef FUSION_H
#define FUSION_H

#include "Particula.h"

namespace Fusion {
inline bool chocan(const Particula& a, const Particula& b) {
    (void)a; (void)b;
    return false; // Persona A implementará la detección real
}

inline void fusionar(Particula& a, Particula& b) {
    (void)a; (void)b; // Persona A implementará la conservación de momento
}
}

#endif