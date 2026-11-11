#ifndef LECTORSIMULACION_H
#define LECTORSIMULACION_H

#include <string>
#include <vector>
#include "Caja.h"
#include "Obstaculo.h"

struct PosicionParticula {
    int id;
    double x, y;
};

struct FrameSimulacion {
    double tiempo;
    std::vector<PosicionParticula> posiciones;
};

class LectorSimulacion {
private:
    Caja caja;
    std::vector<obstaculo> obstaculos;
    std::vector<FrameSimulacion> frames;

public:
    LectorSimulacion() = default;

    bool cargarArchivo(const std::string& ruta);

    const Caja& getCaja() const { return caja; }
    const std::vector<obstaculo>& getObstaculos() const { return obstaculos; }
    const std::vector<FrameSimulacion>& getFrames() const { return frames; }
};

#endif