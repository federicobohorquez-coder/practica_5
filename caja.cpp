#include "Caja.h"
bool Caja::rebotar(Particula& p) const {
    bool reboto = false;
    const double r = p.getRadio();

    // Pared izquierda
    if (p.getX() - r < xmin) {
        p.setX(xmin + r);               // reubicar justo en el borde
        if (p.getVx() < 0) {            // solo hay rebote si se estaba acercando
            p.setVx(-p.getVx());        // v'_perp = -v_perp (elastico)
            reboto = true;
        }
    }
    // Pared derecha
    else if (p.getX() + r > xmax) {
        p.setX(xmax - r);
        if (p.getVx() > 0) {
            p.setVx(-p.getVx());
            reboto = true;
        }
    }

    // Pared ymin
    if (p.getY() - r < ymin) {
        p.setY(ymin + r);
        if (p.getVy() < 0) {
            p.setVy(-p.getVy());
            reboto = true;
        }
    }
    // Pared ymax
    else if (p.getY() + r > ymax) {
        p.setY(ymax - r);
        if (p.getVy() > 0) {
            p.setVy(-p.getVy());
            reboto = true;
        }
    }

    // Los ejes x e y se evaluan por separado: en una esquina se invierten
    // ambas componentes. La componente paralela a la pared no se toca.
    return reboto;
}