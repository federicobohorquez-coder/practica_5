#ifndef PARTICULA_H
#define PARTICULA_H

class Particula {
private:
    int id;
    double x, y;
    double vx, vy;
    double masa;
    double radio;
    bool activa;

public:
    Particula(int id, double x, double y, double vx, double vy, double masa, double radio);

    void mover(double dt);

    // Getters
    int getId() const { return id; }
    double getX() const { return x; }
    double getY() const { return y; }
    double getVx() const { return vx; }
    double getVy() const { return vy; }
    double getMasa() const { return masa; }
    double getRadio() const { return radio; }
    bool isActiva() const { return activa; }

    // Setters
    void setX(double nx) { x = nx; }
    void setY(double ny) { y = ny; }
    void setVx(double nvx) { vx = nvx; }
    void setVy(double nvy) { vy = nvy; }
    void setActiva(bool estado) { activa = estado; }
};

#endif