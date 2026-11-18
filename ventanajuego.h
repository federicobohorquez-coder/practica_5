#ifndef VENTANAJUEGO_H
#define VENTANAJUEGO_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <vector>
#include <unordered_map>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsSimpleTextItem>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "proyectil.h"
#include "caja.h"
#include "bloque.h"
#include "Jugador.h"

class VentanaJuego : public QMainWindow {
    Q_OBJECT

public:
    explicit VentanaJuego(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void tic();   // se ejecuta cada 10 ms
        void actualizarCanion();   // gira el cañón cuando se mueve un slider

private:
    QGraphicsScene *scene;
    QGraphicsView *view;

    // TEMPORAL: mientras Salomé termina la lógica, la ventana arma su propio escenario.
    Caja caja;
    std::vector<Bloque> bloques;
    std::vector<Jugador> jugadores;

    // id del bloque -> su rectángulo en pantalla (igual que con las partículas en la Parte 1)
    std::unordered_map<int, QGraphicsRectItem*> itemsBloques;
    QGraphicsRectItem *canones[2];
    std::unordered_map<int, QGraphicsSimpleTextItem*> textosBloques;  // número de resistencia
    std::unordered_map<int, QGraphicsRectItem*> sombrasBloques;       // capa roja de daño

    void actualizarBloques();

    void crearEscenario();
    void dibujarEscena();

    QTimer *timer;
    Proyectil proyectil;
    bool enVuelo = false;
    double tiempoVuelo = 0.0;
    int turno = 0;

    QGraphicsEllipseItem *itemProyectil;
    QGraphicsPathItem *trayectoria;                  // línea punteada del tiro
    QPainterPath camino;
    std::vector<QGraphicsEllipseItem*> marcasRebote; // puntos amarillos donde rebotó

    void disparar(double angulo, double velocidad);
    void marcarRebote(double x, double y);


    QSlider *sliderAngulo;
    QSlider *sliderVelocidad;
    QLabel *lblAngulo;
    QLabel *lblVelocidad;
    QLabel *lblTurno;
    QPushButton *btnDisparar;

    void actualizarPanel();
    int ganador = -1;                          // -1 = nadie ha ganado
    QGraphicsRectItem *bannerVictoria;         // franja oscura semitransparente
    QGraphicsSimpleTextItem *textoVictoria;    // "¡Gana Jugador X!"
    QGraphicsSimpleTextItem *textoReiniciar;   // "Presiona R para jugar de nuevo"

    void mostrarVictoria(int jugador);
    void reiniciarPartida();
    bool tocaRival(int jugadorRival) const;    // TEMPORAL
    QSoundEffect *sonidoDisparo;     // efecto corto, suena al instante
    QMediaPlayer *musicaAmbiente;    // música larga en bucle
    QAudioOutput *salidaAudio;       // por dónde sale el audio del QMediaPlayer (Qt 6)
};

#endif