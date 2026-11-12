#include "LectorSimulacion.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool LectorSimulacion::cargarArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo: " << ruta << std::endl;
        return false;
    }

    frames.clear();
    obstaculos.clear();

    std::string linea;
    FrameSimulacion frameActual{-1.0, {}};

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        std::string tipo;
        std::getline(ss, tipo, ',');

        if (tipo == "CAJA") {
            double xmin, xmax, ymin, ymax;
            char c;
            ss >> xmin >> c >> xmax >> c >> ymin >> c >> ymax;
            caja = Caja(xmin, xmax, ymin, ymax);
        }
        else if (tipo == "OBS") {
            int id;
            double x, y, lado;
            char c;
            ss >> id >> c >> x >> c >> y >> c >> lado;
            obstaculos.emplace_back(id, x, y, lado);
        }
        else if (tipo == "POS") {
            double t, x, y, r = 3.0;
            int id;
            char c;
            ss >> t >> c >> id >> c >> x >> c >> y >> c >> r;

            if (frameActual.tiempo != t) {
                if (frameActual.tiempo >= 0.0) {
                    frames.push_back(frameActual);
                }
                frameActual.tiempo = t;
                frameActual.posiciones.clear();
            }
            frameActual.posiciones.push_back({id, x, y, r});
        }
    }

    if (frameActual.tiempo >= 0.0) {
        frames.push_back(frameActual);
    }

    archivo.close();
    return true;
}