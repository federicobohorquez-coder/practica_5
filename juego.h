#ifndef JUEGO_H
#define JUEGO_H

#include <vector>
#include "Config.h"
#include "Caja.h"
#include "Bloque.h"
#include "Proyectil.h"
#include "Representante.h"
#include "Jugador.h"

enum class EstadoJuego {
    ESPERANDO_LANZAMIENTO,   // el jugador del turno debe elegir angulo y velocidad
    PROYECTIL_EN_VUELO,
    TERMINADO                // hay un ganador
};

// Informacion del ultimo choque que hizo dano (para mostrar mensajes o efectos)
struct ImpactoInfo {
    bool valido = false;
    int idBloque = -1;
    int propietarioBloque = -1;
    double velocidadImpacto = 0.0;
    double momentoLineal = 0.0;
    double dano = 0.0;
    double resistenciaRestante = 0.0;
    bool destruido = false;
};

class Juego {
private:
    Caja caja;
    std::vector<Bloque> bloques;
    std::vector<Representante> representantes;   // [0] y [1]
    std::vector<Jugador> jugadores;              // [0] y [1]

    Proyectil proyectil;
    bool hayProyectilEnVuelo;
    double tiempoVuelo;
    int impactosEnTurno;      // choques con bloques del proyectil actual

    EstadoJuego estado;
    int turno;               // jugador al que le toca (0 o 1)
    int numeroTurno;         // empieza en 1
    int ganador;             // -1 mientras no haya
    ImpactoInfo ultimoImpacto;

    void crearEscenario();
    bool procesarBloques();        // choques con la infraestructura + dano
    bool verificarVictoria();
    void terminarDisparo();        // cambia de turno

public:
    Juego();

    // Acciones (las llama la interfaz)
    // Dispara con el jugador del turno. Devuelve false si no es posible
    // (hay un proyectil en vuelo, el juego termino o angulo/velocidad fuera de rango).
    bool lanzar(double anguloGrados, double velocidad);

    // Avanza la simulacion un paso de tiempo (llamar desde un QTimer).
    void actualizar(double dt = Config::DT);

    void reiniciar();

    // Formula del daño (publica para poder probarla y explicarla)
    static double calcularDano(double masa, double velocidadImpacto) {
        return Config::FACTOR_DANO * masa * velocidadImpacto;
    }

    // Consultas para la interfaz
    EstadoJuego getEstado() const { return estado; }
    int getTurnoActual() const { return turno; }
    int getNumeroTurno() const { return numeroTurno; }
    int getGanador() const { return ganador; }
    bool hayProyectil() const { return hayProyectilEnVuelo; }
    const Proyectil& getProyectil() const { return proyectil; }
    const Caja& getCaja() const { return caja; }
    const std::vector<Bloque>& getBloques() const { return bloques; }
    const Representante& getRepresentante(int jugador) const { return representantes[jugador]; }
    const Jugador& getJugador(int id) const { return jugadores[id]; }
    const ImpactoInfo& getUltimoImpacto() const { return ultimoImpacto; }
    int bloquesRestantes(int jugador) const;
};

#endif // JUEGO_H