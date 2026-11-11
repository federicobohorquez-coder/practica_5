#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <vector>
#include <unordered_map>
#include "LectorSimulacion.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    QTimer *timer;

    LectorSimulacion lector;
    size_t frameActualIndex;

    // Almacena las representaciones visuales de las partículas por su ID
    std::unordered_map<int, QGraphicsEllipseItem*> itemsParticulas;

    void cargarYDibujarEstaticos();
    void inicializarParticulas();

private slots:
    void avanzarFrame();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;
};

#endif