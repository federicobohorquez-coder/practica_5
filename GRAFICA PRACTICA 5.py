"""
Grafica la simulacion de colisiones multiples a partir del archivo de salida
(simulacion_salida.txt) generado por el programa 
"""
import sys
from collections import defaultdict

import matplotlib.pyplot as plt
import matplotlib.patches as patches

# ---------------- Configuracion ----------------
import os
CARPETA = os.path.dirname(os.path.abspath(__file__))   # carpeta donde esta ESTE script
NOMBRES = ("simulacion_salida.txt", "simulacion_salida_ejemplo.txt")

def buscar_archivo():
    """Usa la ruta dada por consola; si no, busca el archivo junto al script
    y en la carpeta desde la que se ejecuta"""
    if len(sys.argv) > 1:
        return sys.argv[1]
    for base in (CARPETA, os.getcwd()):
        for nombre in NOMBRES:
            ruta = os.path.join(base, nombre)
            if os.path.exists(ruta):
                return ruta
    sys.exit("No encontre el archivo de salida. Busque en:\n  " + CARPETA +
             "\n  " + os.getcwd() + "\nNombres buscados: " + ", ".join(NOMBRES) +
             "\nPonga el archivo en alguna de esas carpetas o escriba su ruta en RUTA.")

RUTA = buscar_archivo()
print("Leyendo:", RUTA)
INVERTIR_Y = False        # True si quieren ver el eje Y hacia abajo (como en Qt)
ETIQUETAR_TODOS = False   # True: pone el tiempo en TODAS las colisiones (puede saturar)
# -----------------------------------------------

caja = None
obstaculos = []                                   # (id, x, y, lado)
trayectorias = defaultdict(lambda: ([], [], []))  # id -> (t, x, y)
colisiones = []                                   # (tipo, t, id1, id2, x, y)

# Formato del archivo (una linea por registro, valores separados por comas):
#   CAJA,xmin,xmax,ymin,ymax
#   OBS,id,x,y,lado
#   POS,t,id,x,y
#   COL,tipo,t,id1,id2,x,y        tipo = PARED | OBST | PART
with open(RUTA) as f:
    for linea in f:
        c = linea.strip().split(",")
        if not c or c[0] == "":
            continue
        if c[0] == "CAJA":
            caja = tuple(float(v) for v in c[1:5])
        elif c[0] == "OBS":
            obstaculos.append((int(c[1]), float(c[2]), float(c[3]), float(c[4])))
        elif c[0] == "POS":
            t, pid, x, y = float(c[1]), int(c[2]), float(c[3]), float(c[4])
            trayectorias[pid][0].append(t)
            trayectorias[pid][1].append(x)
            trayectorias[pid][2].append(y)
        elif c[0] == "COL":
            colisiones.append((c[1], float(c[2]), int(c[3]), int(c[4]),
                               float(c[5]), float(c[6])))

if caja is None:
    sys.exit("El archivo no tiene la linea CAJA: revisa la ruta o el formato.")

xmin, xmax, ymin, ymax = caja
fig, ax = plt.subplots(figsize=(11, 7))

# Caja y obstaculos
ax.add_patch(patches.Rectangle((xmin, ymin), xmax - xmin, ymax - ymin,
                               fill=False, edgecolor="black", linewidth=2))
for oid, x, y, lado in obstaculos:
    ax.add_patch(patches.Rectangle((x, y), lado, lado, facecolor="lightgray",
                                   edgecolor="dimgray"))
    ax.text(x + lado / 2, y + lado / 2, f"O{oid}", ha="center", va="center", fontsize=8)

# Trayectorias (una por particula) con su punto de inicio
for pid in sorted(trayectorias):
    _, xs, ys = trayectorias[pid]
    linea, = ax.plot(xs, ys, linewidth=1.3, label=f"Partícula {pid}")
    ax.plot(xs[0], ys[0], "o", color=linea.get_color(), markersize=7)

# Colisiones: simbolo distinto por tipo
estilos = {
    "PARED": dict(marker=".", color="black", s=30, label="Choque con pared (elástico)"),
    "OBST":  dict(marker="x", color="red",   s=55, label="Choque con obstáculo (inelástico)"),
    "PART":  dict(marker="*", color="green", s=170, label="Fusión de partículas"),
}
for tipo, est in estilos.items():
    pts = [(x, y) for (tp, t, a, b, x, y) in colisiones if tp == tipo]
    if pts:
        ax.scatter([p[0] for p in pts], [p[1] for p in pts], zorder=5, **est)

# Instante de tiempo de las colisiones
for (tp, t, a, b, x, y) in colisiones:
    if tp == "PART" or ETIQUETAR_TODOS:
        ax.annotate(f"t={t:.2f} s", (x, y), textcoords="offset points",
                    xytext=(6, 6), fontsize=8)

ax.set_title("Simulación de colisiones múltiples")
ax.set_xlabel("Posición X")
ax.set_ylabel("Posición Y")
ax.set_aspect("equal")
ax.set_xlim(xmin - 2, xmax + 2)
ax.set_ylim(ymin - 2, ymax + 2)
ax.grid(True)
if INVERTIR_Y:
    ax.invert_yaxis()   # igual que en el ejemplo de la profesora
ax.legend(loc="upper center", bbox_to_anchor=(0.5, -0.08), ncol=3, fontsize=8)
fig.tight_layout()
fig.savefig("simulacion_grafica.png", dpi=150)

# Segunda figura: instante de tiempo de cada colision (una fila por particula)
fig2, ax2 = plt.subplots(figsize=(11, 3.5))
for tipo, est in estilos.items():
    pts = [(t, a) for (tp, t, a, b, x, y) in colisiones if tp == tipo]
    if pts:
        ax2.scatter([p[0] for p in pts], [p[1] for p in pts], zorder=5,
                    marker=est["marker"], color=est["color"], s=est["s"] * 0.8,
                    label=est["label"])
ax2.set_title("Instante de cada colisión")
ax2.set_xlabel("Tiempo t (s)")
ax2.set_ylabel("Partícula (id)")
ax2.set_yticks(sorted(trayectorias))
ax2.grid(True)
ax2.legend(loc="upper center", bbox_to_anchor=(0.5, -0.28), ncol=3, fontsize=8)
fig2.tight_layout()
fig2.savefig("colisiones_tiempo.png", dpi=150)

# Resumen en consola: colisiones con su instante de tiempo
print(f"{'tipo':6} {'t (s)':>8}  {'id1':>3} {'id2':>3}   x        y")
for (tp, t, a, b, x, y) in colisiones:
    print(f"{tp:6} {t:8.2f}  {a:3d} {b:3d}  {x:7.2f}  {y:7.2f}")
print(f"\nTotal: {len(colisiones)} colisiones "
      f"({sum(1 for c in colisiones if c[0]=='PARED')} pared, "
      f"{sum(1 for c in colisiones if c[0]=='OBST')} obstáculo, "
      f"{sum(1 for c in colisiones if c[0]=='PART')} fusión).")
plt.show()