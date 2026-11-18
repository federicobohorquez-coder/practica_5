#include "Representante.h"
#include <algorithm>

bool Representante::tocadoPor(const Particula& p) const {
    const double cx = std::max(x, std::min(p.getX(), x + ancho));   // punto del rectangulo
    const double cy = std::max(y, std::min(p.getY(), y + alto));    // mas cercano al centro
    const double dx = p.getX() - cx;
    const double dy = p.getY() - cy;
    return dx * dx + dy * dy < p.getRadio() * p.getRadio();
}