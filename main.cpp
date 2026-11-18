#include <QApplication>
#include "MainWindow.h"
#include "Simulacion.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 1. Configurar la simulación: dt = 0.01s, tiempoTotal = 10s, restitución e = 0.8
    Simulacion sim(0.01, 10.0, 0.8, "simulacion_salida.txt");

    // 2. Definir la caja (ancho: 200, alto: 150)
    sim.setCaja(Caja(0.0, 200.0, 0.0, 150.0));

    // 3. Agregar los 4 obstáculos cuadrados estáticos (id, x, y, lado)
    sim.agregarObstaculo(obstaculo(1, 45.0,  30.0, 15.0));
    sim.agregarObstaculo(obstaculo(2, 140.0, 30.0, 15.0));
    sim.agregarObstaculo(obstaculo(3, 45.0,  105.0, 15.0));
    sim.agregarObstaculo(obstaculo(4, 140.0, 105.0, 15.0));

    // 4. Agregar 4 partículas simultáneas (id, x, y, vx, vy, masa, radio)
    // Partículas 1 y 2 van de frente hacia el centro para fusionarse
    sim.agregarParticula(Particula(1, 20.0,  75.0,  35.0,  5.0, 2.0, 3.5));
    sim.agregarParticula(Particula(2, 180.0, 78.0, -35.0, -3.0, 3.0, 4.0));
    // Partícula 3 choca contra el obstáculo 1 y rebota inelásticamente
    sim.agregarParticula(Particula(3, 52.0,  10.0,   4.0, 40.0, 1.5, 3.0));
    // Partícula 4 rebota entre paredes y obstáculos inferiores
    sim.agregarParticula(Particula(4, 110.0, 135.0, 32.0, -38.0, 2.5, 3.5));

    // 5. Ejecutar motor físico y generar "simulacion_salida.txt"
    sim.ejecutar();

    // 6. Abrir interfaz gráfica y reproducir la simulación
    MainWindow window;
    window.setWindowTitle("Simulador de Colisiones Múltiples - Práctica 5");
    window.show();

    return app.exec();
}