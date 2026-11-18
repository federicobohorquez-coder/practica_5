#include "ventanajuego.h"
#include "Config.h"
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <cmath>
#include <algorithm>

// ======================== CONSTRUCTOR ========================
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
    sliderAngulo->setRange(static_cast<int>(Config::ANGULO_MIN),
                           static_cast<int>(Config::ANGULO_MAX));
    sliderAngulo->setValue(45);
    sliderAngulo->setFocusPolicy(Qt::NoFocus);

    lblVelocidad = new QLabel(central);
    lblVelocidad->setMinimumWidth(110);
    sliderVelocidad = new QSlider(Qt::Horizontal, central);
    sliderVelocidad->setRange(static_cast<int>(Config::VEL_MIN),
                              static_cast<int>(Config::VEL_MAX));
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
    dibujarEscena();   // el escenario ya lo creó el constructor de Juego

    const double r = Config::RADIO_PROYECTIL;
    itemProyectil = scene->addEllipse(-r, -r, 2 * r, 2 * r,
                                      QPen(Qt::black, 1), QBrush(Qt::darkGray));
    itemProyectil->setZValue(5);
    itemProyectil->setVisible(false);
    trayectoria = scene->addPath(QPainterPath(), QPen(Qt::darkBlue, 1, Qt::DashLine));

    // Banner de victoria (oculto hasta que alguien gane)
    bannerVictoria = scene->addRect(0, 0, Config::ANCHO_CAJA, 110,
                                    Qt::NoPen, QBrush(QColor(0, 0, 0, 160)));
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
    textoReiniciar = new QGraphicsSimpleTextItem("Presiona R para jugar de nuevo",
                                                 bannerVictoria);
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

    // ---------- Sonido ----------
    sonidoDisparo = new QSoundEffect(this);
    sonidoDisparo->setSource(QUrl("qrc:/assets/disparo.wav"));
    sonidoDisparo->setVolume(0.6f);

    salidaAudio = new QAudioOutput(this);
    salidaAudio->setVolume(0.3f);               // bajito, para que no tape el disparo
    musicaAmbiente = new QMediaPlayer(this);
    musicaAmbiente->setAudioOutput(salidaAudio);
    musicaAmbiente->setSource(QUrl("qrc:/assets/ambiente.mp3"));
    musicaAmbiente->setLoops(QMediaPlayer::Infinite);   // se repite sin parar
    musicaAmbiente->play();

    QTimer::singleShot(0, this, [this]() {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    });
}

// ======================== DIBUJO INICIAL ========================
void VentanaJuego::dibujarEscena() {
    const Caja &caja = juego.getCaja();
    const double W = caja.xmax - caja.xmin;
    const double H = caja.ymax - caja.ymin;

    const double ALTO_SUELO = 30.0;
    scene->setSceneRect(caja.xmin, caja.ymin, W, H + ALTO_SUELO);   // incluye el suelo

    // Fondo: imagen de 1600x800 px escalada a 800x400 unidades
    QPixmap fondo(":/assets/fondo.png");
    QGraphicsPixmapItem *itemFondo = scene->addPixmap(fondo);
    itemFondo->setTransformationMode(Qt::SmoothTransformation);
    itemFondo->setScale(W / fondo.width());
    itemFondo->setZValue(-10);                                     // detrás de todo

    // Suelo: textura que se repite debajo de la caja
    scene->addRect(caja.xmin, H, W, ALTO_SUELO, Qt::NoPen,
                   QBrush(QPixmap(":/assets/suelo.png")));

    // Borde de la caja (sin relleno, para que se vea el fondo)
    scene->addRect(caja.xmin, caja.ymin, W, H, QPen(Qt::black, 2), Qt::NoBrush);

    QFont fuente;
    fuente.setPixelSize(9);
    fuente.setBold(true);

    QPixmap texLadrillo(":/assets/ladrillo.png");
    for (const Bloque &b : juego.getBloques()) {
        QGraphicsRectItem *item = scene->addRect(0, 0, b.ancho, b.alto,
                                                 QPen(Qt::black, 1), QBrush(texLadrillo));
        item->setPos(b.x, b.y);
        itemsBloques[b.id] = item;

        // Capa roja encima del bloque: transparente al inicio, se oscurece con el daño
        QGraphicsRectItem *sombra = new QGraphicsRectItem(0, 0, b.ancho, b.alto, item);
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

    // Rivales: el sprite se escala a la altura del Representante (80) y se centra en su ancho,
    // así lo que se ve coincide con la zona que detecta Representante::tocadoPor
    for (int p = 0; p < 2; ++p) {
        const Representante &rep = juego.getRepresentante(p);
        QPixmap pix(p == 0 ? ":/assets/rival_azul.png" : ":/assets/rival_rojo.png");
        const double escala = rep.alto / pix.height();
        QGraphicsPixmapItem *rival = scene->addPixmap(pix);
        rival->setTransformationMode(Qt::SmoothTransformation);
        rival->setScale(escala);
        rival->setPos(rep.x + (rep.ancho - pix.width() * escala) / 2.0, rep.y);
    }

    // Cañones: tubo (rectángulo que gira) + base circular
    for (int i = 0; i < 2; ++i) {
        const Jugador &j = juego.getJugador(i);
        QColor color = (i == 0) ? QColor(60, 130, 240) : QColor(235, 70, 70);

        canones[i] = scene->addRect(0, -3, 28, 6, QPen(Qt::black, 1),
                                    QBrush(color.darker(150)));
        canones[i]->setPos(j.getCanionX(), j.getCanionY());
        // Qt gira en sentido horario (y hacia abajo): derecha-arriba = -45, izquierda = 180+45
        canones[i]->setRotation(j.getDireccion() > 0 ? -45.0 : 225.0);

        QGraphicsEllipseItem *baseCanion = scene->addEllipse(-10, -10, 20, 20,
                                                             QPen(Qt::black, 1),
                                                             QBrush(color));
        baseCanion->setPos(j.getCanionX(), j.getCanionY());
    }
}

// ======================== REFRESCO DE BLOQUES ========================
void VentanaJuego::actualizarBloques() {
    for (const Bloque &b : juego.getBloques()) {
        QGraphicsRectItem *item = itemsBloques[b.id];

        // Destruido: se oculta, no se borra (igual que las partículas fusionadas)
        if (b.estaDestruido()) {
            item->setVisible(false);
            continue;
        }
        item->setVisible(true);

        // fraccion = 1 intacto, 0 destruido
        const double fraccion = b.resistencia / b.resistenciaMax;

        // Mientras más daño, más opaca la capa roja (alpha de 0 a 170)
        sombrasBloques[b.id]->setBrush(
            QColor(200, 0, 0, static_cast<int>((1.0 - fraccion) * 170)));

        // Número de resistencia centrado en el bloque
        QGraphicsSimpleTextItem *t = textosBloques[b.id];
        t->setText(QString::number(static_cast<int>(std::ceil(b.resistencia))));
        const QRectF br = t->boundingRect();
        t->setPos((b.ancho - br.width()) / 2.0, (b.alto - br.height()) / 2.0);
    }
}

// ======================== DISPARO ========================
void VentanaJuego::disparar(double angulo, double velocidad) {
    // Juego::lanzar valida todo: que no haya proyectil en vuelo, que no haya ganador y el rango
    if (!juego.lanzar(angulo, velocidad)) return;

    // Borrar las marcas de rebote del tiro anterior
    for (QGraphicsEllipseItem *m : marcasRebote) {
        scene->removeItem(m);
        delete m;
    }
    marcasRebote.clear();

    sonidoDisparo->play();

    const Proyectil &p = juego.getProyectil();
    camino = QPainterPath(QPointF(p.getX(), p.getY()));
    trayectoria->setPath(camino);
    itemProyectil->setPos(p.getX(), p.getY());
    itemProyectil->setVisible(true);
    actualizarPanel();                 // bloquea los controles mientras vuela
}

// ======================== BUCLE DEL JUEGO ========================
void VentanaJuego::tic() {
    if (juego.getEstado() != EstadoJuego::PROYECTIL_EN_VUELO) return;

    const int turnoAntes = juego.getTurnoActual();
    const Proyectil &p = juego.getProyectil();
    const double vxAntes = p.getVx();
    const double vyAntes = p.getVy();

    // 1. Un paso de física (clase de Salomé): mover, rebote ELÁSTICO con la caja (3.2),
    //    choque inelástico y daño (3.4, 3.5), victoria y fin del turno
    juego.actualizar(Config::DT);

    // 2. Marca amarilla si rebotó en una pared: la velocidad cambió de signo pegada a la caja
    const Caja &caja = juego.getCaja();
    const double r = p.getRadio();
    const bool enParedX = (p.getX() - r <= caja.xmin + 0.01) || (p.getX() + r >= caja.xmax - 0.01);
    const bool enParedY = (p.getY() - r <= caja.ymin + 0.01) || (p.getY() + r >= caja.ymax - 0.01);
    if ((enParedX && vxAntes * p.getVx() < 0) || (enParedY && vyAntes * p.getVy() < 0)) {
        marcarRebote(p.getX(), p.getY());
    }

    // 3. Refrescar el dibujo: daño en bloques, proyectil y trayectoria
    actualizarBloques();
    itemProyectil->setPos(p.getX(), p.getY());
    camino.lineTo(p.getX(), p.getY());
    trayectoria->setPath(camino);

    // 4. ¿Alguien ganó?
    if (juego.getEstado() == EstadoJuego::TERMINADO) {
        mostrarVictoria(juego.getGanador());
        return;
    }

    // 5. ¿Terminó el tiro? (Juego cambia el turno)
    if (juego.getTurnoActual() != turnoAntes) {
        itemProyectil->setVisible(false);

        // El siguiente jugador arranca con su último ángulo y velocidad
        const Jugador &j = juego.getJugador(juego.getTurnoActual());
        sliderAngulo->setValue(static_cast<int>(j.getUltimoAngulo()));
        sliderVelocidad->setValue(static_cast<int>(j.getUltimaVelocidad()));
        actualizarCanion();
        actualizarPanel();
    }
}

void VentanaJuego::marcarRebote(double x, double y) {
    QGraphicsEllipseItem *m = scene->addEllipse(-3, -3, 6, 6, QPen(Qt::black, 0.5),
                                                QBrush(Qt::yellow));
    m->setPos(x, y);
    m->setZValue(4);
    marcasRebote.push_back(m);
}

// ======================== TECLADO Y CONTROLES ========================
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

    if (juego.getEstado() != EstadoJuego::ESPERANDO_LANZAMIENTO) return;  // no gira en vuelo
    const int turno = juego.getTurnoActual();
    const int dir = juego.getJugador(turno).getDireccion();
    // Qt gira en sentido horario (y hacia abajo): derecha-arriba = -a, izquierda = 180 + a
    canones[turno]->setRotation(dir > 0 ? -a : 180.0 + a);
}

void VentanaJuego::actualizarPanel() {
    const bool puedeDisparar = juego.getEstado() == EstadoJuego::ESPERANDO_LANZAMIENTO;
    btnDisparar->setEnabled(puedeDisparar);
    sliderAngulo->setEnabled(puedeDisparar);
    sliderVelocidad->setEnabled(puedeDisparar);

    const int ganador = juego.getGanador();
    const int jugador = (ganador >= 0) ? ganador : juego.getTurnoActual();
    QColor color = (jugador == 0) ? QColor(60, 130, 240) : QColor(235, 70, 70);
    const QString nombre = QString::fromStdString(juego.getJugador(jugador).getNombre());

    lblTurno->setText(ganador >= 0 ? QString("Ganó %1").arg(nombre)
                                   : QString("Turno: %1").arg(nombre));
    lblTurno->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 16px;")
                                .arg(color.name()));
}

// ======================== VICTORIA Y REINICIO ========================
void VentanaJuego::mostrarVictoria(int jugador) {
    itemProyectil->setVisible(false);

    QColor color = (jugador == 0) ? QColor(90, 160, 255) : QColor(255, 100, 100);
    textoVictoria->setText(QString("¡Gana %1!")
                               .arg(QString::fromStdString(juego.getJugador(jugador).getNombre())));
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
    juego.reiniciar();               // bloques con resistencia completa, turno 0, sin ganador

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

// ======================== ESCALADO ========================
void VentanaJuego::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    QTimer::singleShot(0, this, [this]() {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    });
}
