#include "Juego.h"

Juego::Juego()
    : caja(0.0, Config::ANCHO_CAJA, 0.0, Config::ALTO_CAJA),
    hayProyectilEnVuelo(false), tiempoVuelo(0.0), impactosEnTurno(0),
    estado(EstadoJuego::ESPERANDO_LANZAMIENTO), turno(0), numeroTurno(1), ganador(-1) {
    crearEscenario();
}

void Juego::reiniciar() {
    *this = Juego();
}

// Escenario como la Figura 2: cada jugador tiene un bloque superior (resistencia 100),
// dos bloques laterales (200) y su representante en medio; el canon esta junto a la pared.
void Juego::crearEscenario() {
    bloques.clear();
    representantes.clear();
    jugadores.clear();

    const double suelo = Config::ALTO_CAJA;
    for (int j = 0; j < 2; ++j) {
        const double x0 = (j == 0) ? 80.0 : Config::ANCHO_CAJA - 80.0 - 220.0;   // base de 220 de ancho
        const int base = j * 10;
        bloques.push_back(Bloque(base + 1, j, x0,         suelo - 120.0,  60.0, 120.0, Config::RESISTENCIA_LATERAL));   // lateral izq.
        bloques.push_back(Bloque(base + 2, j, x0 + 160.0, suelo - 120.0,  60.0, 120.0, Config::RESISTENCIA_LATERAL));   // lateral der.
        bloques.push_back(Bloque(base + 3, j, x0,         suelo - 180.0, 220.0,  60.0, Config::RESISTENCIA_SUPERIOR)); // superior
        representantes.push_back(Representante(j, x0 + 85.0, suelo - 80.0, 50.0, 80.0));
    }
    jugadores.push_back(Jugador(0, "Jugador 1", 30.0, 200.0, +1));
    jugadores.push_back(Jugador(1, "Jugador 2", Config::ANCHO_CAJA - 30.0, 200.0, -1));
}

bool Juego::lanzar(double anguloGrados, double velocidad) {
    if (estado != EstadoJuego::ESPERANDO_LANZAMIENTO) return false;
    if (anguloGrados < Config::ANGULO_MIN || anguloGrados > Config::ANGULO_MAX) return false;
    if (velocidad < Config::VEL_MIN || velocidad > Config::VEL_MAX) return false;

    const Jugador& j = jugadores[turno];
    proyectil = Proyectil(turno, j.getCanionX(), j.getCanionY(), anguloGrados, velocidad,
                          j.getDireccion(), Config::MASA_PROYECTIL, Config::RADIO_PROYECTIL);
    jugadores[turno].registrarLanzamiento(anguloGrados, velocidad);
    hayProyectilEnVuelo = true;
    tiempoVuelo = 0.0;
    impactosEnTurno = 0;
    ultimoImpacto = ImpactoInfo();
    estado = EstadoJuego::PROYECTIL_EN_VUELO;
    return true;
}

void Juego::actualizar(double dt) {
    if (estado != EstadoJuego::PROYECTIL_EN_VUELO) return;

    // 1. Mover con gravedad
    proyectil.avanzar(dt, Config::GRAVEDAD);
    tiempoVuelo += dt;

    // 2. Paredes de la caja: rebote perfectamente elastico (punto 3.2)
    caja.rebotar(proyectil);

    // 3. Infraestructura: rebote inelastico (3.4) y dano (3.5)
    const bool choqueConBloque = procesarBloques();

    // 4. Victoria
    if (verificarVictoria()) return;

    // 5. Fin del turno
    const bool casiDetenido = choqueConBloque && proyectil.rapidez() < Config::VEL_MIN_IMPACTO;
    if (tiempoVuelo >= Config::TIEMPO_MAX_VUELO || casiDetenido ||
        impactosEnTurno >= Config::MAX_IMPACTOS_TURNO) terminarDisparo();
}

bool Juego::procesarBloques() {
    bool hubo = false;
    for (Bloque& b : bloques) {
        if (b.estaDestruido()) continue;

        double velImpacto = 0.0;
        if (!b.colisiona(proyectil, Config::COEF_RESTITUCION, velImpacto)) continue;   // 3.4
        hubo = true;
        ++impactosEnTurno;

        // Solo la infraestructura del RIVAL recibe dano, y solo si el choque es lo bastante fuerte
        if (b.propietario != turno && velImpacto >= Config::VEL_MIN_IMPACTO) {
            const double dano = calcularDano(proyectil.getMasa(), velImpacto);
            b.recibirDano(dano);

            ultimoImpacto.valido = true;
            ultimoImpacto.idBloque = b.id;
            ultimoImpacto.propietarioBloque = b.propietario;
            ultimoImpacto.velocidadImpacto = velImpacto;
            ultimoImpacto.momentoLineal = proyectil.getMasa() * velImpacto;
            ultimoImpacto.dano = dano;
            ultimoImpacto.resistenciaRestante = b.resistencia;
            ultimoImpacto.destruido = b.estaDestruido();
        }
    }
    return hubo;
}

bool Juego::verificarVictoria() {
    Representante& rival = representantes[1 - turno];
    if (rival.vivo && rival.tocadoPor(proyectil)) {
        rival.vivo = false;
        ganador = turno;
        estado = EstadoJuego::TERMINADO;
        hayProyectilEnVuelo = false;
        return true;
    }
    return false;
}

void Juego::terminarDisparo() {
    hayProyectilEnVuelo = false;
    turno = 1 - turno;
    ++numeroTurno;
    estado = EstadoJuego::ESPERANDO_LANZAMIENTO;
}

int Juego::bloquesRestantes(int jugador) const {
    int n = 0;
    for (const Bloque& b : bloques)
        if (b.propietario == jugador && !b.estaDestruido()) ++n;
    return n;
}