#ifndef VENTANAJUEGO_H
#define VENTANAJUEGO_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsSimpleTextItem>
#include <QPainterPath>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <vector>
#include <unordered_map>
#include "juego.h"

class VentanaJuego : public QMainWindow {
    Q_OBJECT

public:
    explicit VentanaJuego(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void tic();                // se ejecuta cada 10 ms
    void actualizarCanion();   // gira el cañón cuando se mueve un slider

private:
    // ---- Escena ----
    QGraphicsScene *scene;
    QGraphicsView *view;

    // Lógica del juego (clase de Salomé): física, turnos, daño y victoria
    Juego juego;

    // id del bloque -> su rectángulo en pantalla (igual que con las partículas en la Parte 1)
    std::unordered_map<int, QGraphicsRectItem*> itemsBloques;
    std::unordered_map<int, QGraphicsSimpleTextItem*> textosBloques;  // número de resistencia
    std::unordered_map<int, QGraphicsRectItem*> sombrasBloques;       // capa roja de daño
    QGraphicsRectItem *canones[2];

    // ---- Disparo y animación ----
    QTimer *timer;

    QGraphicsEllipseItem *itemProyectil;
    QGraphicsPathItem *trayectoria;                  // línea punteada del tiro
    QPainterPath camino;
    std::vector<QGraphicsEllipseItem*> marcasRebote; // puntos amarillos donde rebotó

    // ---- Panel de control ----
    QSlider *sliderAngulo;
    QSlider *sliderVelocidad;
    QLabel *lblAngulo;
    QLabel *lblVelocidad;
    QLabel *lblTurno;
    QPushButton *btnDisparar;

    // ---- Victoria ----
    QGraphicsRectItem *bannerVictoria;         // franja oscura semitransparente
    QGraphicsSimpleTextItem *textoVictoria;    // "¡Gana Jugador X!"
    QGraphicsSimpleTextItem *textoReiniciar;   // "Presiona R para jugar de nuevo"

    // ---- Sonido ----
    QSoundEffect *sonidoDisparo;     // efecto corto, suena al instante
    QMediaPlayer *musicaAmbiente;    // música larga en bucle
    QAudioOutput *salidaAudio;       // por dónde sale el audio del QMediaPlayer (Qt 6)

    // ---- Funciones internas ----
    void dibujarEscena();
    void actualizarBloques();
    void disparar(double angulo, double velocidad);
    void marcarRebote(double x, double y);
    void actualizarPanel();
    void mostrarVictoria(int jugador);
    void reiniciarPartida();
};

#endif
