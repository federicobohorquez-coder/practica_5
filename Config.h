#ifndef CONFIG_H
#define CONFIG_H

namespace Config {

//Escenario
const double ANCHO_CAJA = 800.0;
const double ALTO_CAJA  = 400.0;

//  Fisica del proyectil
const double GRAVEDAD          = 98.1;   // unidades/s^2 (1 unidad = 0.1 m  ->  9.81 m/s^2)
const double MASA_PROYECTIL    = 2.0;
const double RADIO_PROYECTIL   = 5.0;
const double COEF_RESTITUCION  = 0.5;    // e: choque inelastico con la infraestructura

// Daño (3.5)
//   dano = FACTOR_DAñO * masa * |v_impacto|
const double FACTOR_DANO       = 0.1;
const double VEL_MIN_IMPACTO   = 10.0;   // por debajo de esta rapidez el choque no hace dano

//  Lanzamiento
const double ANGULO_MIN = 0.0;           // grados
const double ANGULO_MAX = 90.0;
const double VEL_MIN    = 20.0;          // unidades/s
const double VEL_MAX    = 350.0;

// Simulacion
const double DT               = 0.01;    // paso de tiempo (s)
const double TIEMPO_MAX_VUELO = 8.0;     // el turno termina si el proyectil vuela mas que esto (s)
const int    MAX_IMPACTOS_TURNO = 2;     // el turno termina tras este numero de choques con bloques
// (evita que un solo tiro destruya y gane por rebotes)

//Infraestructura
const double RESISTENCIA_SUPERIOR = 100.0;   // bloque de arriba
const double RESISTENCIA_LATERAL  = 200.0;   // bloques de los lados

}

#endif
