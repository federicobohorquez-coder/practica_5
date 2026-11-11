#include "Obstaculo.h"
#include <algorithm>
#include <cmath>

bool obstaculo::colisiona(Particula& p, double e) const {
    const double r  = p.getRadio();
    const double px = p.getX();
    const double py = p.getY();

    // 1. DETECCION: punto del cuadrado mas cercano al centro de la particula
    const double cx = std::max(x, std::min(px, x + lado));
    const double cy = std::max(y, std::min(py, y + lado));
    const double dx = px - cx;
    const double dy = py - cy;
    if (dx * dx + dy * dy >= r * r) return false;   // no se tocan

    // 2. LADO GOLPEADO: normal n (unitaria, apunta hacia afuera del cuadrado)
    double nx = 0.0, ny = 0.0;
    if (dx != 0.0 || dy != 0.0) {
        if (std::fabs(dx) > std::fabs(dy)) nx = (dx > 0) ? 1.0 : -1.0;  // lado de mayor/menor x
        else                               ny = (dy > 0) ? 1.0 : -1.0;  // lado de mayor/menor y
    } else {
        // centro dentro del cuadrado: se usa el lado mas cercano
        const double izq = px - x,        der = x + lado - px;
        const double aba = py - y,        arr = y + lado - py;
        const double m = std::min(std::min(izq, der), std::min(aba, arr));
        if      (m == izq) nx = -1.0;
        else if (m == der) nx =  1.0;
        else if (m == aba) ny = -1.0;
        else               ny =  1.0;
    }

    // 3. Reubicar la particula fuera del cuadrado (tocando el lado)
    if (nx != 0.0) p.setX((nx > 0 ? x + lado : x) + nx * r);
    else           p.setY((ny > 0 ? y + lado : y) + ny * r);

    // 4. Solo hay rebote si la particula se acerca al lado (v . n < 0)
    const double vn = p.getVx() * nx + p.getVy() * ny;
    if (vn >= 0.0) return false;

    // 5. REBOTE INELASTICO: v' = v_par - e * v_perp = v - (1 + e) * vn * n
    p.setVx(p.getVx() - (1.0 + e) * vn * nx);
    p.setVy(p.getVy() - (1.0 + e) * vn * ny);
    return true;
}