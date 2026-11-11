#include "MainWindow.h"
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), frameActualIndex(0)
{
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    setCentralWidget(view);
    resize(900, 600);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::avanzarFrame);

    if (lector.cargarArchivo("simulacion_salida.txt")) {
        cargarYDibujarEstaticos();
        inicializarParticulas();
        // Dispara la animación cada 10 ms (coincide con el dt = 0.01s)
        timer->start(10);
    } else {
        std::cerr << "No se pudo cargar la simulación en la GUI." << std::endl;
    }
}

void MainWindow::cargarYDibujarEstaticos() {
    const Caja& caja = lector.getCaja();

    // Dibujar los bordes de la Caja
    double ancho = caja.xmax - caja.xmin;
    double alto = caja.ymax - caja.ymin;
    scene->setSceneRect(caja.xmin, caja.ymin, ancho, alto);
    scene->addRect(caja.xmin, caja.ymin, ancho, alto, QPen(Qt::white, 2), QBrush(Qt::black));

    // Dibujar los Obstáculos
    for (const auto& obs : lector.getObstaculos()) {
        scene->addRect(obs.x, obs.y, obs.lado, obs.lado,
                       QPen(Qt::darkRed, 1), QBrush(Qt::red));
    }
}

void MainWindow::inicializarParticulas() {
    const auto& frames = lector.getFrames();
    if (frames.empty()) return;

    // Tomamos el primer frame para crear los círculos en pantalla
    const auto& primerFrame = frames[0];
    for (const auto& pos : primerFrame.posiciones) {
        double radio = 1.5; // Tamaño visual de la partícula
        QGraphicsEllipseItem* item = scene->addEllipse(-radio, -radio, radio * 2, radio * 2,
                                                       QPen(Qt::cyan), QBrush(Qt::cyan));
        item->setPos(pos.x, pos.y);
        itemsParticulas[pos.id] = item;
    }
}

void MainWindow::avanzarFrame() {
    const auto& frames = lector.getFrames();
    if (frameActualIndex >= frames.size()) {
        timer->stop(); // Fin de la animación
        return;
    }

    const auto& frameActual = frames[frameActualIndex];
    for (const auto& pos : frameActual.posiciones) {
        if (itemsParticulas.count(pos.id)) {
            itemsParticulas[pos.id]->setPos(pos.x, pos.y);
        }
    }

    frameActualIndex++;
}