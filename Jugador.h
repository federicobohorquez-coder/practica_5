#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
class Jugador {
private:
    int id;
    std::string nombre;
    double canionX, canionY;
    int direccion;
    int lanzamientos;
    double ultimoAngulo, ultimaVelocidad;

public:
    Jugador(int id = 0, const std::string& nombre = "", double canionX = 0,
            double canionY = 0, int direccion = 1)
        : id(id), nombre(nombre), canionX(canionX), canionY(canionY),
        direccion(direccion), lanzamientos(0), ultimoAngulo(45.0), ultimaVelocidad(150.0) {}

    int getId() const { return id; }
    const std::string& getNombre() const { return nombre; }
    double getCanionX() const { return canionX; }
    double getCanionY() const { return canionY; }
    int getDireccion() const { return direccion; }
    int getLanzamientos() const { return lanzamientos; }
    double getUltimoAngulo() const { return ultimoAngulo; }
    double getUltimaVelocidad() const { return ultimaVelocidad; }

    void registrarLanzamiento(double angulo, double velocidad) {
        ++lanzamientos;
        ultimoAngulo = angulo;
        ultimaVelocidad = velocidad;
    }
};

#endif