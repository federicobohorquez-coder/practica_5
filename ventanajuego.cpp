#include "ventanajuego.h"
#include "Config.h"
#include <QTimer>
#include <QPen>
#include <QBrush>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QFont>
#include <QPainterPath>
#include <cmath>
#include <algorithm>

VentanaJuego::VentanaJuego(QWidget *parent) : QMainWindow(parent) {
    scene = new QGraphicsScene(this);

    // ---------- Ventana: escena arriba, controles abajo ----------
    QWidget *central = new QWidget(this);

    view = new QGraphicsView(scene, central);
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFocusPolicy(Qt::NoFocus);

    lblTurno = new QLabel(central);
    lblTurno->setMinimumWidth(150);

    lblAngulo = new QLabel(central);
    lblAngulo->setMinimumWidth(90);
    sliderAngulo = new QSlider(Qt::Horizontal, central);
    sliderAngulo->setRange(static_cast<int>(Config::ANGULO_MIN), static_cast<int>(Config::ANGULO_MAX));
    sliderAngulo->setValue(45);
    sliderAngulo->setFocusPolicy(Qt::NoFocus);

    lblVelocidad = new QLabel(central);
    lblVelocidad->setMinimumWidth(110);
    sliderVelocidad = new QSlider(Qt::Horizontal, central);
    sliderVelocidad->setRange(static_cast<int>(Config::VEL_MIN), static_cast<int>(Config::VEL_MAX));
    sliderVelocidad->setValue(150);
    sliderVelocidad->setFocusPolicy(Qt::NoFocus);

    btnDisparar = new QPushButton("Disparar", central);
    btnDisparar->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *controles = new QHBoxLayout();
    controles->addWidget(lblTurno);
    controles->addWidget(lblAngulo);
    controles->addWidget(sliderAngulo, 1);
    controles->addWidget(lblVelocidad);
    controles->addWidget(sliderVelocidad, 1);
    controles->addWidget(btnDisparar);

    QVBoxLayout *principal = new QVBoxLayout(central);
    principal->addWidget(view, 1);       // el 1 hace que la escena ocupe el espacio sobrante
    principal->addLayout(controles);
    setCentralWidget(central);
    resize(1000, 600);

    // ---------- Escena ----------
    crearEscenario();
    dibujarEscena();

    const double r = Config::RADIO_PROYECTIL;
    itemProyectil = scene->addEllipse(-r, -r, 2 * r, 2 * r, QPen(Qt::black, 1), QBrush(Qt::darkGray));
    itemProyectil->setZValue(5);
    itemProyectil->setVisible(false);
    trayectoria = scene->addPath(QPainterPath(), QPen(Qt::darkBlue, 1, Qt::DashLine));

    // Banner de victoria (oculto hasta que alguien gane)
    bannerVictoria = scene->addRect(0, 0, Config::ANCHO_CAJA, 110, Qt::NoPen, QBrush(QColor(0, 0, 0, 160)));
    bannerVictoria->setPos(0, 110);
    bannerVictoria->setZValue(20);            // encima de todo
    bannerVictoria->setVisible(false);

    QFont fuenteGrande;
    fuenteGrande.setPixelSize(42);
    fuenteGrande.setBold(true);
    textoVictoria = new QGraphicsSimpleTextItem(bannerVictoria);   // hijo del banner
    textoVictoria->setFont(fuenteGrande);

    QFont fuenteChica;
    fuenteChica.setPixelSize(16);
    textoReiniciar = new QGraphicsSimpleTextItem("Presiona R para jugar de nuevo", bannerVictoria);
    textoReiniciar->setFont(fuenteChica);
    textoReiniciar->setBrush(Qt::white);

    // ---------- Señales y slots ----------
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VentanaJuego::tic);
    connect(sliderAngulo, &QSlider::valueChanged, this, &VentanaJuego::actualizarCanion);
    connect(sliderVelocidad, &QSlider::valueChanged, this, &VentanaJuego::actualizarCanion);
    connect(btnDisparar, &QPushButton::clicked, this, [this]() {
        disparar(sliderAngulo->value(), sliderVelocidad->value());
    });
    timer->start(10);

    setFocusPolicy(Qt::StrongFocus);
    actualizarCanion();
    actualizarPanel();

    QTimer::singleShot(0, this, [this]() {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    });
}
void VentanaJuego::crearEscenario() {
    const double W = Config::ANCHO_CAJA;   // 800
    const double H = Config::ALTO_CAJA;    // 400
    caja = Caja(0.0, W, 0.0, H);

    // Fortalezas como la figura 2: superior (100) y dos laterales (200)
    const double xFort[2] = {70.0, W - 70.0 - 140.0};
    for (int p = 0; p < 2; ++p) {
        const double x0 = xFort[p];
        const int base = p * 10;   // ids: jugador 0 -> 0,1,2 ; jugador 1 -> 10,11,12
        bloques.emplace_back(base + 0, p, x0,         H - 100, 140, 20, Config::RESISTENCIA_SUPERIOR);
        bloques.emplace_back(base + 1, p, x0,         H - 80,  20,  80, Config::RESISTENCIA_LATERAL);
        bloques.emplace_back(base + 2, p, x0 + 120.0, H - 80,  20,  80, Config::RESISTENCIA_LATERAL);
    }

    // Cañones afuera de cada fortaleza, sobre el suelo
    const double yCanion = H - Config::RADIO_PROYECTIL;
    jugadores.emplace_back(0, "Jugador 1", xFort[0] + 140 + 40, yCanion,  1);  // dispara a la derecha
    jugadores.emplace_back(1, "Jugador 2", xFort[1] - 40,       yCanion, -1);  // dispara a la izquierda
}

void VentanaJuego::dibujarEscena() {
    const double W = caja.xmax - caja.xmin;
    const double H = caja.ymax - caja.ymin;
    scene->setSceneRect(caja.xmin, caja.ymin, W, H);

    // Fondo y borde de la caja
    scene->addRect(caja.xmin, caja.ymin, W, H, QPen(Qt::black, 2), QBrush(QColor(170, 210, 240)));

    QFont fuente;
    fuente.setPixelSize(9);
    fuente.setBold(true);

    for (const Bloque &b : bloques) {
        QGraphicsRectItem *item = scene->addRect(0, 0, b.ancho, b.alto,
                                                 QPen(Qt::black, 1), QBrush(QColor(180, 100, 60)));
        item->setPos(b.x, b.y);
        itemsBloques[b.id] = item;

        // Capa roja encima del bloque: transparente al inicio, se oscurece con el daño
        QGraphicsRectItem *sombra = new QGraphicsRectItem(0, 0, b.ancho, b.alto, item);  // hijo
        sombra->setPen(Qt::NoPen);
        sombra->setBrush(Qt::transparent);
        sombrasBloques[b.id] = sombra;

        // Número de resistencia (hijo: se mueve y se oculta junto con el bloque)
        QGraphicsSimpleTextItem *texto = new QGraphicsSimpleTextItem(item);
        texto->setFont(fuente);
        texto->setBrush(Qt::white);
        textosBloques[b.id] = texto;
    }
    actualizarBloques();   // pone los números iniciales (100 y 200) centrados

    // Rivales (solo dibujo por ahora; su lógica la define Salomé)
    for (int p = 0; p < 2; ++p) {
        const Bloque &sup = bloques[p * 3];   // bloque superior de esa fortaleza
        QColor color = (p == 0) ? QColor(60, 130, 240) : QColor(235, 70, 70);
        QGraphicsRectItem *rival = scene->addRect(0, 0, 40, 45, QPen(Qt::black, 1), QBrush(color));
        rival->setPos(sup.x + 50, H - 45);
        QGraphicsSimpleTextItem *t = new QGraphicsSimpleTextItem("Rival", rival);
        t->setFont(fuente);
        t->setBrush(Qt::white);
        t->setPos((40 - t->boundingRect().width()) / 2.0, 15);
    }

    // Cañones: tubo (rectángulo que gira) + base circular
    for (int i = 0; i < 2; ++i) {
        const Jugador &j = jugadores[i];
        QColor color = (i == 0) ? QColor(60, 130, 240) : QColor(235, 70, 70);

        canones[i] = scene->addRect(0, -3, 28, 6, QPen(Qt::black, 1), QBrush(color.darker(150)));
        canones[i]->setPos(j.getCanionX(), j.getCanionY());
        // Qt gira en sentido horario (y hacia abajo): derecha-arriba = -45, izquierda-arriba = 180+45
        canones[i]->setRotation(j.getDireccion() > 0 ? -45.0 : 225.0);

        QGraphicsEllipseItem *baseCanion = scene->addEllipse(-10, -10, 20, 20,
                                                             QPen(Qt::black, 1), QBrush(color));
        baseCanion->setPos(j.getCanionX(), j.getCanionY());
    }
}

void VentanaJuego::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    QTimer::singleShot(0, this, [this]() {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    });
}
void VentanaJuego::disparar(double angulo, double velocidad) {
    if (enVuelo || ganador >= 0) return;   // no se dispara en vuelo ni si ya hay ganador

    // Borrar las marcas de rebote del tiro anterior
    for (QGraphicsEllipseItem *m : marcasRebote) {
        scene->removeItem(m);
        delete m;
    }
    marcasRebote.clear();

    const Jugador &j = jugadores[turno];
    proyectil = Proyectil(turno, j.getCanionX(), j.getCanionY(), angulo, velocidad,
                          j.getDireccion(), Config::MASA_PROYECTIL, Config::RADIO_PROYECTIL);
    enVuelo = true;
    tiempoVuelo = 0.0;

    camino = QPainterPath(QPointF(proyectil.getX(), proyectil.getY()));
    trayectoria->setPath(camino);
    itemProyectil->setPos(proyectil.getX(), proyectil.getY());
    itemProyectil->setVisible(true);
    jugadores[turno].registrarLanzamiento(angulo, velocidad);   // Jugador recuerda su último tiro
    actualizarPanel();                                          // bloquea los controles mientras vuela
}

void VentanaJuego::tic() {
    if (!enVuelo) return;

    // 1. Movimiento parabólico (ecuaciones del enunciado, en Proyectil::avanzar)
    proyectil.avanzar(Config::DT, Config::GRAVEDAD);
    tiempoVuelo += Config::DT;

    // 2. COLISIÓN ELÁSTICA con las paredes de la caja (punto 3.2)
    if (caja.rebotar(proyectil)) {
        marcarRebote(proyectil.getX(), proyectil.getY());
    }
    // 2b. TEMPORAL: choque inelástico con bloques y daño.
    //     Esto lo reemplaza la clase de Salomé; aquí solo sirve para probar el dibujo.
    for (Bloque &b : bloques) {
        if (b.estaDestruido()) continue;
        double vImpacto = 0.0;
        if (b.colisiona(proyectil, Config::COEF_RESTITUCION, vImpacto)) {
            if (b.propietario != turno && vImpacto >= Config::VEL_MIN_IMPACTO) {
                b.recibirDano(Config::FACTOR_DANO * proyectil.getMasa() * vImpacto);
            }
            actualizarBloques();   // ← ESTA es tu parte: refrescar el dibujo después del daño
            break;
        }
    }
    // 2c. TEMPORAL: victoria si el proyectil toca al Rival enemigo (esto lo decide Salomé)
    if (tocaRival(1 - turno)) {
        mostrarVictoria(turno);
        return;
    }

    // 3. Actualizar el dibujo con la nueva posición
    itemProyectil->setPos(proyectil.getX(), proyectil.getY());
    camino.lineTo(proyectil.getX(), proyectil.getY());
    trayectoria->setPath(camino);

    // 4. TEMPORAL: el tiro termina a los 8 s y cambia el turno (después lo decide Salomé)
    if (tiempoVuelo >= Config::TIEMPO_MAX_VUELO) {
        enVuelo = false;
        itemProyectil->setVisible(false);
        turno = 1 - turno;

        // El siguiente jugador arranca con su último ángulo y velocidad
        sliderAngulo->setValue(static_cast<int>(jugadores[turno].getUltimoAngulo()));
        sliderVelocidad->setValue(static_cast<int>(jugadores[turno].getUltimaVelocidad()));
        actualizarCanion();
        actualizarPanel();
    }
}

void VentanaJuego::marcarRebote(double x, double y) {
    QGraphicsEllipseItem *m = scene->addEllipse(-3, -3, 6, 6, QPen(Qt::black, 0.5), QBrush(Qt::yellow));
    m->setPos(x, y);
    m->setZValue(4);
    marcasRebote.push_back(m);
}

void VentanaJuego::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Space: disparar(sliderAngulo->value(), sliderVelocidad->value()); break;
    case Qt::Key_Up:    sliderAngulo->setValue(sliderAngulo->value() + 1); break;
    case Qt::Key_Down:  sliderAngulo->setValue(sliderAngulo->value() - 1); break;
    case Qt::Key_Right: sliderVelocidad->setValue(sliderVelocidad->value() + 5); break;
    case Qt::Key_Left:  sliderVelocidad->setValue(sliderVelocidad->value() - 5); break;
    case Qt::Key_R:     reiniciarPartida(); break;
    default:            QMainWindow::keyPressEvent(event);
    }
}

void VentanaJuego::actualizarCanion() {
    const int a = sliderAngulo->value();
    lblAngulo->setText(QString("Ángulo: %1°").arg(a));
    lblVelocidad->setText(QString("Velocidad: %1").arg(sliderVelocidad->value()));

    if (enVuelo) return;   // mientras vuela, el cañón no se mueve
    const int dir = jugadores[turno].getDireccion();
    // Qt gira en sentido horario (y hacia abajo): derecha-arriba = -a, izquierda-arriba = 180 + a
    canones[turno]->setRotation(dir > 0 ? -a : 180.0 + a);
}

void VentanaJuego::actualizarPanel() {
    const bool puedeDisparar = !enVuelo && ganador < 0;
    btnDisparar->setEnabled(puedeDisparar);
    sliderAngulo->setEnabled(puedeDisparar);
    sliderVelocidad->setEnabled(puedeDisparar);

    const int jugador = (ganador >= 0) ? ganador : turno;
    QColor color = (jugador == 0) ? QColor(60, 130, 240) : QColor(235, 70, 70);
    const QString nombre = QString::fromStdString(jugadores[jugador].getNombre());

    lblTurno->setText(ganador >= 0 ? QString("Ganó %1").arg(nombre)
                                   : QString("Turno: %1").arg(nombre));
    lblTurno->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 16px;").arg(color.name()));
}

void VentanaJuego::actualizarBloques() {
    for (const Bloque &b : bloques) {
        QGraphicsRectItem *item = itemsBloques[b.id];

        // Destruido: se oculta, no se borra (igual que las partículas fusionadas en la Parte 1)
        if (b.estaDestruido()) {
            item->setVisible(false);
            continue;
        }
        item->setVisible(true);

        // fraccion = 1 intacto, 0 destruido
        const double fraccion = b.resistencia / b.resistenciaMax;

        // Mientras más daño, más opaca la capa roja (alpha de 0 a 170)
        sombrasBloques[b.id]->setBrush(QColor(200, 0, 0, static_cast<int>((1.0 - fraccion) * 170)));

        // Número de resistencia centrado en el bloque
        QGraphicsSimpleTextItem *t = textosBloques[b.id];
        t->setText(QString::number(static_cast<int>(std::ceil(b.resistencia))));
        const QRectF br = t->boundingRect();
        t->setPos((b.ancho - br.width()) / 2.0, (b.alto - br.height()) / 2.0);
    }
}
void VentanaJuego::mostrarVictoria(int jugador) {
    ganador = jugador;
    enVuelo = false;
    itemProyectil->setVisible(false);

    QColor color = (jugador == 0) ? QColor(90, 160, 255) : QColor(255, 100, 100);
    textoVictoria->setText(QString("¡Gana %1!").arg(QString::fromStdString(jugadores[jugador].getNombre())));
    textoVictoria->setBrush(color);
    textoVictoria->setPen(QPen(Qt::black, 1));

    // Centrar los textos dentro del banner (coordenadas locales del banner)
    const double W = Config::ANCHO_CAJA;
    textoVictoria->setPos((W - textoVictoria->boundingRect().width()) / 2.0, 15);
    textoReiniciar->setPos((W - textoReiniciar->boundingRect().width()) / 2.0, 75);

    bannerVictoria->setVisible(true);
    actualizarPanel();
}

void VentanaJuego::reiniciarPartida() {
    // TEMPORAL: cuando esté la clase de Salomé, aquí se llama a su reiniciar()
    bloques.clear();
    jugadores.clear();
    crearEscenario();                // vuelve a crear bloques con resistencia completa

    ganador = -1;
    turno = 0;
    enVuelo = false;

    bannerVictoria->setVisible(false);
    itemProyectil->setVisible(false);
    trayectoria->setPath(QPainterPath());
    for (QGraphicsEllipseItem *m : marcasRebote) {
        scene->removeItem(m);
        delete m;
    }
    marcasRebote.clear();

    canones[0]->setRotation(-45.0);
    canones[1]->setRotation(225.0);
    sliderAngulo->setValue(45);
    sliderVelocidad->setValue(150);

    actualizarBloques();             // vuelve a mostrar los bloques destruidos
    actualizarCanion();
    actualizarPanel();
}

// TEMPORAL: misma detección que Bloque::colisiona (punto del rectángulo más cercano al centro)
bool VentanaJuego::tocaRival(int jugadorRival) const {
    const Bloque &sup = bloques[jugadorRival * 3];        // bloque superior de esa fortaleza
    const double rx = sup.x + 50, ry = caja.ymax - 45;    // misma posición con la que se dibujó
    const double rw = 40, rh = 45;

    const double cx = std::max(rx, std::min(proyectil.getX(), rx + rw));
    const double cy = std::max(ry, std::min(proyectil.getY(), ry + rh));
    const double dx = proyectil.getX() - cx;
    const double dy = proyectil.getY() - cy;
    const double r = proyectil.getRadio();
    return dx * dx + dy * dy < r * r;
}