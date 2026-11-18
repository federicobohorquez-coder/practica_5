#include "bloque.h"
#include <algorithm>
#include <cmath>

void Bloque::recibirDano(double dano) {
    resistencia -= dano;
    if (resistencia < 0.0) resistencia = 0.0;
}

bool Bloque::colisiona(Particula& p, double e, double& velocidadImpacto) const {
    const double r  = p.getRadio();
    const double px = p.getX();
    const double py = p.getY();

    // 1. DETECCION: punto del rectangulo mas cercano al centro de la particula
    const double cx = std::max(x, std::min(px, x + ancho));
    const double cy = std::max(y, std::min(py, y + alto));
    const double dx = px - cx;
    const double dy = py - cy;
    if (dx * dx + dy * dy >= r * r) return false;        // no se tocan

    // 2. LADO GOLPEADO: normal n (unitaria, apunta hacia afuera del rectangulo)
    double nx = 0.0, ny = 0.0;
    if (dx != 0.0 || dy != 0.0) {
        if (std::fabs(dx) > std::fabs(dy)) nx = (dx > 0) ? 1.0 : -1.0;   // lado izquierdo/derecho
        else                               ny = (dy > 0) ? 1.0 : -1.0;   // lado de arriba/abajo
    } else {
        // el centro quedo dentro del rectangulo: se usa el lado mas cercano
        const double dIzq  = px - x,         dDer  = x + ancho - px;
        const double dArr  = py - y,         dAba  = y + alto - py;      // y crece hacia abajo
        const double m = std::min(std::min(dIzq, dDer), std::min(dArr, dAba));
        if      (m == dIzq) nx = -1.0;
        else if (m == dDer) nx =  1.0;
        else if (m == dArr) ny = -1.0;
        else                ny =  1.0;
    }

    // 3. REUBICAR la particula fuera del rectangulo, tocando el lado golpeado
    if (nx != 0.0) p.setX((nx > 0 ? x + ancho : x) + nx * r);
    else           p.setY((ny > 0 ? y + alto  : y) + ny * r);

    // 4. Solo hay rebote si la particula se acerca al lado (v . n < 0)
    const double vn = p.getVx() * nx + p.getVy() * ny;
    if (vn >= 0.0) return false;

    // Rapidez de impacto: |v| antes de cambiar la velocidad (la usa el dano)
    velocidadImpacto = std::sqrt(p.getVx() * p.getVx() + p.getVy() * p.getVy());

    // 5. REBOTE INELASTICO: v' = v_par - e * v_perp = v - (1 + e) * (v . n) * n
    p.setVx(p.getVx() - (1.0 + e) * vn * nx);
    p.setVy(p.getVy() - (1.0 + e) * vn * ny);
    return true;
}