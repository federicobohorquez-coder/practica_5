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
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(view);
    resize(900, 600);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::avanzarFrame);

    if (lector.cargarArchivo("simulacion_salida.txt")) {
        cargarYDibujarEstaticos();
        inicializarParticulas();

        // Espera a que Qt termine de dimensionar la ventana para aplicar el zoom real
        QTimer::singleShot(0, this, [this]() {
            view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        });

        timer->start(10);
    } else {
        std::cerr << "No se pudo cargar la simulación en la GUI." << std::endl;
    }
}

void MainWindow::cargarYDibujarEstaticos() {
    const Caja& caja = lector.getCaja();

    double ancho = caja.xmax - caja.xmin;
    double alto = caja.ymax - caja.ymin;
    scene->setSceneRect(caja.xmin, caja.ymin, ancho, alto);
    scene->addRect(caja.xmin, caja.ymin, ancho, alto, QPen(Qt::white, 1), QBrush(Qt::black));

    for (const auto& obs : lector.getObstaculos()) {
        scene->addRect(obs.x, obs.y, obs.lado, obs.lado,
                       QPen(Qt::darkRed, 0.5), QBrush(Qt::red));
    }

    // Ajusta el zoom de la vista para que la caja ocupe toda la ventana
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::inicializarParticulas() {
    const auto& frames = lector.getFrames();
    if (frames.empty()) return;

    const auto& primerFrame = frames[0];
    for (const auto& pos : primerFrame.posiciones) {
        double radio = pos.radio; // Usa el radio leído del .txt
        QGraphicsEllipseItem* item = scene->addEllipse(-radio, -radio, radio * 2, radio * 2,
                                                       QPen(Qt::cyan), QBrush(Qt::cyan));
        item->setPos(pos.x, pos.y);
        itemsParticulas[pos.id] = item;
    }
}

void MainWindow::avanzarFrame() {
    const auto& frames = lector.getFrames();
    if (frameActualIndex >= frames.size()) {
        timer->stop();
        return;
    }

    // 1. Ocultar todas primero (así desaparecen las que se hayan fusionado)
    for (auto& par : itemsParticulas) {
        par.second->setVisible(false);
    }

    // 2. Mostrar, redimensionar y mover solo las partículas activas en este frame
    const auto& frameActual = frames[frameActualIndex];
    for (const auto& pos : frameActual.posiciones) {
        if (itemsParticulas.count(pos.id)) {
            itemsParticulas[pos.id]->setVisible(true);
            itemsParticulas[pos.id]->setRect(-pos.radio, -pos.radio, pos.radio * 2, pos.radio * 2);
            itemsParticulas[pos.id]->setPos(pos.x, pos.y);
        }
    }

    frameActualIndex++;
}
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    QTimer::singleShot(0, this, [this]() {
        if (!scene->sceneRect().isEmpty()) {
            view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        }
    });
}