#include "Simulacion.h"
#include <fstream>
#include <iostream>
#include <iomanip>

Simulacion::Simulacion(double dt, double tiempoTotal, double e, const std::string& nombreArchivo)
    : dt(dt), tiempoTotal(tiempoTotal), e(e), nombreArchivo(nombreArchivo) {}

void Simulacion::setCaja(const Caja& c) { caja = c; }
void Simulacion::agregarObstaculo(const Obstaculo& obs) { obstaculos.push_back(obs); }
void Simulacion::agregarParticula(const Particula& p) { particulas.push_back(p); }

void Simulacion::ejecutar() {
    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) return;

    archivo << std::fixed << std::setprecision(4);

    // Cabeceras CAJA y OBS
    archivo << "CAJA," << caja.xmin << "," << caja.xmax << "," << caja.ymin << "," << caja.ymax << "\n";
    for (const auto& obs : obstaculos) {
        archivo << "OBS," << obs.id << "," << obs.x << "," << obs.y << "," << obs.lado << "\n";
    }

    int pasosTotales = static_cast<int>(tiempoTotal / dt);

    for (int paso = 0; paso <= pasosTotales; ++paso) {
        double t = paso * dt;

        // 1. Mover
        for (auto& p : particulas) {
            if (p.isActiva()) p.mover(dt);
        }

        // 2. Paredes
        for (auto& p : particulas) {
            if (p.isActiva() && caja.rebotar(p)) {
                archivo << "COL,PARED," << t << "," << p.getId() << ",-1," << p.getX() << "," << p.getY() << "\n";
            }
        }

        // 3. Obstáculos
        for (auto& p : particulas) {
            if (p.isActiva()) {
                for (const auto& obs : obstaculos) {
                    if (const_cast<Obstaculo&>(obs).colisiona(p, e)) {
                        archivo << "COL,OBST," << t << "," << p.getId() << "," << obs.id << "," << p.getX() << "," << p.getY() << "\n";
                    }
                }
            }
        }

        // 4. Fusión
        size_t n = particulas.size();
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                if (particulas[i].isActiva() && particulas[j].isActiva()) {
                    if (Fusion::chocan(particulas[i], particulas[j])) {
                        double cx = (particulas[i].getX() + particulas[j].getX()) / 2.0;
                        double cy = (particulas[i].getY() + particulas[j].getY()) / 2.0;
                        archivo << "COL,PART," << t << "," << particulas[i].getId() << "," << particulas[j].getId() << "," << cx << "," << cy << "\n";
                        Fusion::fusionar(particulas[i], particulas[j]);
                    }
                }
            }
        }

        // 5. Registro POS
        for (const auto& p : particulas) {
            if (p.isActiva()) {
                archivo << "POS," << t << "," << p.getId() << "," << p.getX() << "," << p.getY() << "\n";
            }
        }
    }

    archivo.close();
    std::cout << "Archivo de salida creado con exito." << std::endl;
}