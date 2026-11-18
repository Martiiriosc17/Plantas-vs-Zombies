/*
 * graficos.c
 * Todo lo que se dibuja en pantalla con raylib.
 *
 * Las funciones de este archivo SOLO dibujan: leen las variables globales
 * del juego (declaradas en juego.h) y las muestran. No cambian ninguna.
 *
 * Todo se dibuja con figuras simples de raylib (rectangulos, circulos,
 * elipses, triangulos y lineas), sin imagenes externas.
 * Las medidas de cada dibujo se calculan como proporcion del tamano de la
 * casilla (por ejemplo s * 0.2f), asi los dibujos se adaptan solos si
 * Jardin.txt trae otra cantidad de filas o columnas.
 */

#include "raylib.h"
#include "juego.h"
#include "graficos.h"

// Vida con la que nacen los zombies. Debe coincidir con generarZombies() en juego.c
#define VIDA_MAXIMA_ZOMBIE 100

// Cuanto dura la animacion de la explosion de la cereza (en segundos)
#define DURACION_EXPLOSION 0.6

/* ---------- Colores propios ---------- */
#define COLOR_PASTO_CLARO      (Color){ 112, 178, 68, 255 }
#define COLOR_PASTO_OSCURO     (Color){ 94, 158, 54, 255 }
#define COLOR_TALLO            (Color){ 46, 125, 50, 255 }
#define COLOR_PETALO           (Color){ 255, 200, 20, 255 }
#define COLOR_CENTRO_GIRASOL   (Color){ 140, 85, 30, 255 }
#define COLOR_GUISANTE         (Color){ 76, 175, 80, 255 }
#define COLOR_GUISANTE_OSCURO  (Color){ 46, 125, 50, 255 }
#define COLOR_METRALLA         (Color){ 46, 140, 60, 255 }
#define COLOR_METRALLA_OSCURO  (Color){ 25, 90, 35, 255 }
#define COLOR_HIELO            (Color){ 125, 205, 240, 255 }
#define COLOR_HIELO_OSCURO     (Color){ 55, 135, 190, 255 }
#define COLOR_NUEZ             (Color){ 190, 125, 60, 255 }
#define COLOR_NUEZ_OSCURO      (Color){ 120, 70, 30, 255 }
#define COLOR_CEREZA           (Color){ 215, 30, 45, 255 }
#define COLOR_CEREZA_OSCURO    (Color){ 140, 15, 30, 255 }
#define COLOR_PIEDRA           (Color){ 150, 150, 155, 255 }
#define COLOR_PIEDRA_OSCURA    (Color){ 95, 95, 100, 255 }
#define COLOR_TIERRA           (Color){ 110, 80, 50, 255 }
#define COLOR_PIEL_ZOMBIE      (Color){ 150, 180, 140, 255 }
#define COLOR_PIEL_CONGELADA   (Color){ 160, 210, 235, 255 }
#define COLOR_ROPA_ZOMBIE      (Color){ 95, 90, 120, 255 }
#define COLOR_ROPA_FLAG        (Color){ 120, 70, 60, 255 }
#define COLOR_ROPA_CONGELADA   (Color){ 90, 120, 170, 255 }
#define COLOR_PANTALON         (Color){ 60, 70, 100, 255 }
#define COLOR_MURO             (Color){ 150, 140, 130, 255 }
#define COLOR_MURO_OSCURO      (Color){ 100, 90, 85, 255 }
#define COLOR_PANEL            (Color){ 55, 55, 60, 255 }
#define COLOR_CARTA            (Color){ 40, 75, 125, 255 }
#define COLOR_BOTON            (Color){ 255, 161, 0, 255 }
#define COLOR_BOTON_ENCIMA     (Color){ 255, 195, 70, 255 }
#define COLOR_CORAZON_PERDIDO  (Color){ 95, 75, 65, 255 }
#define COLOR_METAL            (Color){ 175, 180, 190, 255 }
#define COLOR_MADERA           (Color){ 140, 95, 50, 255 }

/* =====================================================================
 * Funciones auxiliares ('static' = solo se usan dentro de este archivo)
 * ===================================================================== */

// Escribe un texto centrado horizontalmente en centroX.
// MeasureText dice cuantos pixeles de ancho ocupa el texto.
static void textoCentrado(const char *texto, int centroX, int y, int tam, Color color)
{
    int ancho = MeasureText(texto, tam);
    DrawText(texto, centroX - ancho / 2, y, tam, color);
}

static float minimo(float a, float b)
{
    if (a < b) {
        return a;
    }
    return b;
}

// Rectangulo (en pixeles) de la casilla de la fila y columna dadas
static Rectangle rectCasilla(int fila, int columna)
{
    Rectangle r = { pxDistanciaX + columna * anchoCasilla, pxDistanciaY + fila * altoCasilla,
                    anchoCasilla, altoCasilla };
    return r;
}

// Vida con la que nace cada planta, para saber cuanto llenar su barra
static int vidaMaxima(int tipo)
{
    if (tipo == GIRASOL) {
        return VIDA_GIRASOL;
    } else if (tipo == LANZAGUISANTES) {
        return VIDA_LANZAGUISANTES;
    } else if (tipo == NUEZ) {
        return VIDA_NUEZ;
    } else if (tipo == METRALLA) {
        return VIDA_METRALLA;
    } else if (tipo == CONGELADORA) {
        return VIDA_CONGELADORA;
    } else if (tipo == CEREZA) {
        return VIDA_CEREZA;
    }
    return 1;
}

// Costo de cada carta del panel (la pala no tiene costo)
static int costoDe(int tipo)
{
    if (tipo == GIRASOL) {
        return costoGirasol;
    } else if (tipo == LANZAGUISANTES) {
        return costoLanzaguisantes;
    } else if (tipo == NUEZ) {
        return costoNuez;
    } else if (tipo == METRALLA) {
        return costoMetralla;
    } else if (tipo == CONGELADORA) {
        return costoCongeladora;
    } else if (tipo == CEREZA) {
        return costoCereza;
    }
    return 0;
}

// Barra de vida: fondo oscuro y relleno verde, amarillo o rojo segun cuanta vida queda
static void dibujarBarraVida(float x, float y, float ancho, float vida, float vidaMax)
{
    float proporcion = vida / vidaMax;
    Color color;

    if (proporcion < 0) {
        proporcion = 0;
    }
    if (proporcion > 1) {
        proporcion = 1;
    }

    if (proporcion > 0.6f) {
        color = LIME;
    } else if (proporcion > 0.3f) {
        color = GOLD;
    } else {
        color = RED;
    }

    DrawRectangleRec((Rectangle){ x, y, ancho, 7 }, Fade(BLACK, 0.55f));
    DrawRectangleRec((Rectangle){ x + 1, y + 1, (ancho - 2) * proporcion, 5 }, color);
}

// Icono de sol (sin animacion), para el contador y las cartas
static void dibujarIconoSol(Vector2 centro, float radio)
{
    DrawPoly(centro, 3, radio * 1.5f, 0, ORANGE);
    DrawPoly(centro, 3, radio * 1.5f, 60, ORANGE);
    DrawCircleV(centro, radio, GOLD);
    DrawCircleV(centro, radio * 0.6f, YELLOW);
}

// Corazon hecho con dos circulos y un triangulo.
// Ojo: DrawTriangle pide los vertices en sentido contrario a las agujas del reloj.
static void dibujarCorazon(Vector2 c, float r, Color color)
{
    DrawCircleV((Vector2){ c.x - r * 0.55f, c.y - r * 0.2f }, r * 0.62f, color);
    DrawCircleV((Vector2){ c.x + r * 0.55f, c.y - r * 0.2f }, r * 0.62f, color);
    DrawTriangle((Vector2){ c.x - r * 1.15f, c.y - r * 0.05f },
                 (Vector2){ c.x, c.y + r * 1.15f },
                 (Vector2){ c.x + r * 1.15f, c.y - r * 0.05f }, color);
}

/* =====================================================================
 * Plantas
 * Cada una se dibuja dentro del rectangulo r (una casilla o una carta).
 * alfa = 1.0 se ve normal; alfa < 1 se ve transparente (vista previa).
 * ===================================================================== */

static void dibujarGirasol(Rectangle r, float alfa)
{
    float s = minimo(r.width, r.height);
    float cx = r.x + r.width / 2;
    float cy = r.y + r.height / 2 - s * 0.08f;   // la flor va un poco arriba del centro
    float distanciaPetalo = s * 0.21f;
    float radioPetalo = s * 0.11f;
    // Las 8 direcciones de los petalos (0.707 es el coseno de 45 grados)
    float dx[8] = { 1, 0.707f, 0, -0.707f, -1, -0.707f, 0, 0.707f };
    float dy[8] = { 0, 0.707f, 1, 0.707f, 0, -0.707f, -1, -0.707f };
    int i;

    // Tallo y hojas
    DrawRectangleRec((Rectangle){ cx - s * 0.03f, cy, s * 0.06f, s * 0.42f }, Fade(COLOR_TALLO, alfa));
    DrawEllipse((int)(cx - s * 0.12f), (int)(cy + s * 0.3f), s * 0.12f, s * 0.05f, Fade(COLOR_TALLO, alfa));
    DrawEllipse((int)(cx + s * 0.12f), (int)(cy + s * 0.24f), s * 0.12f, s * 0.05f, Fade(COLOR_TALLO, alfa));

    // Petalos alrededor del centro
    for (i = 0; i < 8; i++) {
        Vector2 petalo = { cx + dx[i] * distanciaPetalo, cy + dy[i] * distanciaPetalo };
        DrawCircleV(petalo, radioPetalo, Fade(COLOR_PETALO, alfa));
    }

    // Centro y cara
    DrawCircleV((Vector2){ cx, cy }, s * 0.15f, Fade(COLOR_CENTRO_GIRASOL, alfa));
    DrawCircleV((Vector2){ cx - s * 0.05f, cy - s * 0.03f }, s * 0.022f, Fade(BLACK, alfa));
    DrawCircleV((Vector2){ cx + s * 0.05f, cy - s * 0.03f }, s * 0.022f, Fade(BLACK, alfa));
}

/*
 * Las tres plantas que disparan tienen la misma forma (cabeza redonda con una
 * boca que apunta a la derecha) y cambian en color y en el "peinado":
 *  - LANZAGUISANTES: verde, con una hoja atras
 *  - METRALLA: verde oscuro, con hojas en punta arriba, ceja enojada y doble boca
 *  - CONGELADORA: celeste, con picos de hielo arriba
 */
static void dibujarLanzador(Rectangle r, int tipo, float alfa)
{
    float s = minimo(r.width, r.height);
    float cx = r.x + r.width / 2 - s * 0.06f;
    float cy = r.y + r.height / 2 - s * 0.1f;    // centro de la cabeza
    float radioCabeza = s * 0.19f;
    Color cabeza = Fade(COLOR_GUISANTE, alfa);
    Color oscuro = Fade(COLOR_GUISANTE_OSCURO, alfa);

    if (tipo == METRALLA) {
        cabeza = Fade(COLOR_METRALLA, alfa);
        oscuro = Fade(COLOR_METRALLA_OSCURO, alfa);
    } else if (tipo == CONGELADORA) {
        cabeza = Fade(COLOR_HIELO, alfa);
        oscuro = Fade(COLOR_HIELO_OSCURO, alfa);
    }

    // Tallo y hojas del suelo
    DrawRectangleRec((Rectangle){ cx - s * 0.03f, cy, s * 0.06f, s * 0.44f }, Fade(COLOR_TALLO, alfa));
    DrawEllipse((int)(cx - s * 0.13f), (int)(cy + s * 0.4f), s * 0.14f, s * 0.05f, Fade(COLOR_TALLO, alfa));
    DrawEllipse((int)(cx + s * 0.13f), (int)(cy + s * 0.4f), s * 0.14f, s * 0.05f, Fade(COLOR_TALLO, alfa));

    // "Peinado": va detras de la cabeza, por eso se dibuja primero
    if (tipo == LANZAGUISANTES) {
        DrawEllipse((int)(cx - s * 0.2f), (int)(cy - s * 0.1f), s * 0.1f, s * 0.05f, oscuro);
    } else if (tipo == METRALLA) {
        // Tres hojas en punta (triangulos: punta arriba, base abajo)
        DrawTriangle((Vector2){ cx - s * 0.16f, cy - s * 0.32f }, (Vector2){ cx - s * 0.2f, cy - s * 0.1f }, (Vector2){ cx - s * 0.06f, cy - s * 0.14f }, oscuro);
        DrawTriangle((Vector2){ cx - s * 0.02f, cy - s * 0.36f }, (Vector2){ cx - s * 0.09f, cy - s * 0.14f }, (Vector2){ cx + s * 0.05f, cy - s * 0.14f }, oscuro);
        DrawTriangle((Vector2){ cx + s * 0.12f, cy - s * 0.31f }, (Vector2){ cx + s * 0.03f, cy - s * 0.14f }, (Vector2){ cx + s * 0.16f, cy - s * 0.1f }, oscuro);
    } else if (tipo == CONGELADORA) {
        // Picos de hielo
        Color picos = Fade(RAYWHITE, alfa);
        DrawTriangle((Vector2){ cx - s * 0.14f, cy - s * 0.33f }, (Vector2){ cx - s * 0.19f, cy - s * 0.1f }, (Vector2){ cx - s * 0.07f, cy - s * 0.14f }, picos);
        DrawTriangle((Vector2){ cx - s * 0.01f, cy - s * 0.37f }, (Vector2){ cx - s * 0.08f, cy - s * 0.14f }, (Vector2){ cx + s * 0.06f, cy - s * 0.14f }, picos);
        DrawTriangle((Vector2){ cx + s * 0.13f, cy - s * 0.3f }, (Vector2){ cx + s * 0.04f, cy - s * 0.14f }, (Vector2){ cx + s * 0.17f, cy - s * 0.1f }, picos);
    }

    // Boca: apunta a la derecha, porque los zombies vienen desde ese lado
    DrawRectangleRec((Rectangle){ cx + s * 0.08f, cy - s * 0.07f, s * 0.2f, s * 0.14f }, cabeza);
    DrawCircleV((Vector2){ cx + s * 0.28f, cy }, s * 0.09f, oscuro);
    DrawCircleV((Vector2){ cx + s * 0.28f, cy }, s * 0.05f, Fade(BLACK, alfa * 0.7f));
    if (tipo == METRALLA) {
        // Anillo extra en la boca: dispara mas rapido
        DrawRectangleRec((Rectangle){ cx + s * 0.17f, cy - s * 0.09f, s * 0.04f, s * 0.18f }, oscuro);
    }

    // Cabeza y ojo
    DrawCircleV((Vector2){ cx, cy }, radioCabeza, cabeza);
    DrawCircleV((Vector2){ cx + s * 0.06f, cy - s * 0.06f }, s * 0.045f, Fade(BLACK, alfa));
    DrawCircleV((Vector2){ cx + s * 0.075f, cy - s * 0.075f }, s * 0.015f, Fade(WHITE, alfa));

    if (tipo == METRALLA) {
        // Ceja enojada
        DrawLineEx((Vector2){ cx + s * 0.0f, cy - s * 0.15f }, (Vector2){ cx + s * 0.12f, cy - s * 0.1f }, s * 0.03f, Fade(BLACK, alfa));
    }
}

// La nuez muestra grietas cuando ya perdio vida
static void dibujarNuez(Rectangle r, int vida, float alfa)
{
    float s = minimo(r.width, r.height);
    float cx = r.x + r.width / 2;
    float cy = r.y + r.height / 2 + s * 0.04f;
    float rx = s * 0.27f;   // radio horizontal
    float ry = s * 0.34f;   // radio vertical
    Color oscuro = Fade(COLOR_NUEZ_OSCURO, alfa);

    DrawEllipse((int)cx, (int)cy, rx, ry, Fade(COLOR_NUEZ, alfa));
    DrawEllipseLines((int)cx, (int)cy, rx, ry, oscuro);

    // Ojos (miran a la derecha, hacia los zombies)
    DrawCircleV((Vector2){ cx - s * 0.08f, cy - s * 0.1f }, s * 0.06f, Fade(WHITE, alfa));
    DrawCircleV((Vector2){ cx + s * 0.08f, cy - s * 0.1f }, s * 0.06f, Fade(WHITE, alfa));
    DrawCircleV((Vector2){ cx - s * 0.06f, cy - s * 0.1f }, s * 0.03f, Fade(BLACK, alfa));
    DrawCircleV((Vector2){ cx + s * 0.1f, cy - s * 0.1f }, s * 0.03f, Fade(BLACK, alfa));

    // Boca
    DrawLineEx((Vector2){ cx - s * 0.06f, cy + s * 0.08f }, (Vector2){ cx + s * 0.06f, cy + s * 0.08f }, 2, oscuro);

    // Grietas segun el dano recibido
    if (vida < VIDA_NUEZ * 2 / 3) {
        DrawLineEx((Vector2){ cx + rx * 0.3f, cy - ry * 0.95f }, (Vector2){ cx + rx * 0.05f, cy - ry * 0.55f }, 2, oscuro);
        DrawLineEx((Vector2){ cx + rx * 0.05f, cy - ry * 0.55f }, (Vector2){ cx + rx * 0.35f, cy - ry * 0.3f }, 2, oscuro);
    }
    if (vida < VIDA_NUEZ / 3) {
        DrawLineEx((Vector2){ cx - rx * 0.95f, cy + ry * 0.2f }, (Vector2){ cx - rx * 0.5f, cy + ry * 0.35f }, 2, oscuro);
        DrawLineEx((Vector2){ cx - rx * 0.5f, cy + ry * 0.35f }, (Vector2){ cx - rx * 0.6f, cy + ry * 0.7f }, 2, oscuro);
    }
}

// Dos cerezas unidas por el tallo. 'escala' permite inflarla antes de explotar.
static void dibujarCerezaEn(Rectangle r, float escala, float alfa)
{
    float s = minimo(r.width, r.height) * escala;
    float cx = r.x + r.width / 2;
    float cy = r.y + r.height / 2;
    Vector2 izquierda = { cx - s * 0.15f, cy + s * 0.1f };
    Vector2 derecha = { cx + s * 0.15f, cy + s * 0.14f };
    Vector2 nudo = { cx + s * 0.02f, cy - s * 0.3f };
    float radio = s * 0.17f;
    int k;

    // Tallos y hoja
    DrawLineEx(nudo, (Vector2){ izquierda.x, izquierda.y - radio }, s * 0.03f, Fade(COLOR_TALLO, alfa));
    DrawLineEx(nudo, (Vector2){ derecha.x, derecha.y - radio }, s * 0.03f, Fade(COLOR_TALLO, alfa));
    DrawEllipse((int)(nudo.x + s * 0.08f), (int)(nudo.y - s * 0.02f), s * 0.09f, s * 0.04f, Fade(COLOR_TALLO, alfa));

    // Las dos cerezas, con brillo y cara enojada
    for (k = 0; k < 2; k++) {
        Vector2 c = izquierda;
        if (k == 1) {
            c = derecha;
        }
        DrawCircleV(c, radio, Fade(COLOR_CEREZA, alfa));
        DrawCircleV((Vector2){ c.x - radio * 0.4f, c.y - radio * 0.4f }, radio * 0.22f, Fade(WHITE, alfa * 0.6f));
        DrawCircleV((Vector2){ c.x - radio * 0.3f, c.y }, radio * 0.13f, Fade(BLACK, alfa));
        DrawCircleV((Vector2){ c.x + radio * 0.3f, c.y }, radio * 0.13f, Fade(BLACK, alfa));
        DrawLineEx((Vector2){ c.x - radio * 0.5f, c.y - radio * 0.35f }, (Vector2){ c.x - radio * 0.1f, c.y - radio * 0.18f }, 2, Fade(COLOR_CEREZA_OSCURO, alfa));
        DrawLineEx((Vector2){ c.x + radio * 0.5f, c.y - radio * 0.35f }, (Vector2){ c.x + radio * 0.1f, c.y - radio * 0.18f }, 2, Fade(COLOR_CEREZA_OSCURO, alfa));
    }
}

// Pala: mango de madera con agarre arriba y hoja de metal abajo
static void dibujarPala(Rectangle r, float alfa)
{
    float s = minimo(r.width, r.height);
    float cx = r.x + r.width / 2;
    float arriba = r.y + r.height / 2 - s * 0.38f;
    Color metal = Fade(COLOR_METAL, alfa);

    // Agarre y mango
    DrawRectangleRec((Rectangle){ cx - s * 0.12f, arriba, s * 0.24f, s * 0.06f }, Fade(COLOR_MADERA, alfa));
    DrawRectangleRec((Rectangle){ cx - s * 0.035f, arriba, s * 0.07f, s * 0.42f }, Fade(COLOR_MADERA, alfa));

    // Hoja: rectangulo con punta (triangulo hacia abajo)
    DrawRectangleRec((Rectangle){ cx - s * 0.13f, arriba + s * 0.4f, s * 0.26f, s * 0.2f }, metal);
    DrawTriangle((Vector2){ cx - s * 0.13f, arriba + s * 0.6f },
                 (Vector2){ cx, arriba + s * 0.78f },
                 (Vector2){ cx + s * 0.13f, arriba + s * 0.6f }, metal);
    DrawRectangleRec((Rectangle){ cx - s * 0.13f, arriba + s * 0.4f, s * 0.26f, s * 0.04f }, Fade(DARKGRAY, alfa));
}

// Obstaculo: lapida de piedra sobre un monticulo de tierra
static void dibujarObstaculo(Rectangle r)
{
    float s = minimo(r.width, r.height);
    float cx = r.x + r.width / 2;
    float cy = r.y + r.height / 2;
    Rectangle piedra = { cx - s * 0.22f, cy - s * 0.32f, s * 0.44f, s * 0.58f };
    Rectangle borde = { piedra.x - 3, piedra.y - 3, piedra.width + 6, piedra.height + 6 };

    DrawEllipse((int)cx, (int)(cy + s * 0.28f), s * 0.36f, s * 0.1f, COLOR_TIERRA);
    DrawRectangleRounded(borde, 0.45f, 8, COLOR_PIEDRA_OSCURA);
    DrawRectangleRounded(piedra, 0.45f, 8, COLOR_PIEDRA);

    // Cruz grabada
    DrawRectangleRec((Rectangle){ cx - s * 0.03f, cy - s * 0.2f, s * 0.06f, s * 0.28f }, COLOR_PIEDRA_OSCURA);
    DrawRectangleRec((Rectangle){ cx - s * 0.1f, cy - s * 0.13f, s * 0.2f, s * 0.06f }, COLOR_PIEDRA_OSCURA);

    // Grieta y musgo
    DrawLineEx((Vector2){ piedra.x + piedra.width * 0.8f, piedra.y + piedra.height * 0.55f },
               (Vector2){ piedra.x + piedra.width * 0.65f, piedra.y + piedra.height * 0.75f }, 2, COLOR_PIEDRA_OSCURA);
    DrawEllipse((int)(piedra.x + s * 0.08f), (int)(piedra.y + piedra.height - s * 0.03f), s * 0.08f, s * 0.035f, COLOR_TALLO);
}

// Elige que dibujar segun el codigo de la matriz (o de la carta)
static void dibujarPlanta(int tipo, Rectangle r, int vida, float alfa)
{
    if (tipo == GIRASOL) {
        dibujarGirasol(r, alfa);
    } else if ((tipo == LANZAGUISANTES) || (tipo == METRALLA) || (tipo == CONGELADORA)) {
        dibujarLanzador(r, tipo, alfa);
    } else if (tipo == NUEZ) {
        dibujarNuez(r, vida, alfa);
    } else if (tipo == CEREZA) {
        dibujarCerezaEn(r, 1.0f, alfa);
    } else if (tipo == PALA) {
        dibujarPala(r, alfa);
    }
}

/* =====================================================================
 * Zombies
 * Se dibujan dentro de su rectangulo de choque (x, y, anchoZombie, altoZombie),
 * con los brazos estirados hacia la casa (izquierda).
 * ===================================================================== */

static void dibujarUnZombie(Zombie z, float w, float h)
{
    float x = z.x;
    float y = z.y;
    float cabezaX = x + w * 0.5f;
    float cabezaY = y + h * 0.17f;
    Color piel = COLOR_PIEL_ZOMBIE;
    Color ropa = COLOR_ROPA_ZOMBIE;

    if (z.tipo == FLAG) {
        ropa = COLOR_ROPA_FLAG;
        // Bandera: va detras del cuerpo, por eso se dibuja primero
        DrawRectangleRec((Rectangle){ x + w * 0.82f, y - h * 0.12f, 3, h * 0.9f }, BROWN);
        DrawRectangleRec((Rectangle){ x + w * 0.82f + 3, y - h * 0.12f, w * 0.55f, h * 0.17f }, RED);
        DrawRectangleRec((Rectangle){ x + w * 0.82f + 3, y - h * 0.06f, w * 0.55f, h * 0.04f }, MAROON);
    }
    if (z.congelado == CONGELADO) {
        // Congelado por la congeladora: se pone azulado
        piel = COLOR_PIEL_CONGELADA;
        ropa = COLOR_ROPA_CONGELADA;
    }

    // Piernas
    DrawRectangleRec((Rectangle){ x + w * 0.22f, y + h * 0.72f, w * 0.2f, h * 0.28f }, COLOR_PANTALON);
    DrawRectangleRec((Rectangle){ x + w * 0.58f, y + h * 0.72f, w * 0.2f, h * 0.28f }, COLOR_PANTALON);

    // Cuerpo y corbata
    DrawRectangleRec((Rectangle){ x + w * 0.12f, y + h * 0.33f, w * 0.76f, h * 0.42f }, ropa);
    DrawRectangleRec((Rectangle){ x + w * 0.45f, y + h * 0.35f, w * 0.1f, h * 0.22f }, MAROON);

    // Brazos estirados hacia la izquierda
    DrawRectangleRec((Rectangle){ x - w * 0.25f, y + h * 0.4f, w * 0.45f, h * 0.07f }, piel);
    DrawRectangleRec((Rectangle){ x - w * 0.18f, y + h * 0.5f, w * 0.4f, h * 0.07f }, piel);

    // Cabeza y ojos (mirando a la izquierda)
    DrawCircleV((Vector2){ cabezaX, cabezaY }, w * 0.3f, piel);
    DrawCircleV((Vector2){ cabezaX - w * 0.12f, cabezaY - h * 0.02f }, w * 0.08f, RAYWHITE);
    DrawCircleV((Vector2){ cabezaX + w * 0.07f, cabezaY - h * 0.02f }, w * 0.07f, RAYWHITE);
    DrawCircleV((Vector2){ cabezaX - w * 0.15f, cabezaY - h * 0.02f }, w * 0.035f, BLACK);
    DrawCircleV((Vector2){ cabezaX + w * 0.04f, cabezaY - h * 0.02f }, w * 0.03f, BLACK);

    // Boca: abierta mientras ataca a una planta
    if (z.estado == ATACANDO) {
        DrawCircleV((Vector2){ cabezaX - w * 0.08f, cabezaY + h * 0.08f }, w * 0.07f, MAROON);
    } else {
        DrawLineEx((Vector2){ cabezaX - w * 0.16f, cabezaY + h * 0.08f },
                   (Vector2){ cabezaX + w * 0.02f, cabezaY + h * 0.08f }, 2, DARKGRAY);
    }

    if (z.congelado == CONGELADO) {
        // Copo de nieve sobre la cabeza (tres lineas cruzadas)
        Vector2 copo = { cabezaX + w * 0.3f, y - h * 0.02f };
        float t = w * 0.13f;
        DrawLineEx((Vector2){ copo.x - t, copo.y }, (Vector2){ copo.x + t, copo.y }, 2, RAYWHITE);
        DrawLineEx((Vector2){ copo.x - t * 0.5f, copo.y - t * 0.87f }, (Vector2){ copo.x + t * 0.5f, copo.y + t * 0.87f }, 2, RAYWHITE);
        DrawLineEx((Vector2){ copo.x - t * 0.5f, copo.y + t * 0.87f }, (Vector2){ copo.x + t * 0.5f, copo.y - t * 0.87f }, 2, RAYWHITE);
    }

    // Barra de vida arriba de la cabeza, solo si ya recibio dano
    if (z.vida < VIDA_MAXIMA_ZOMBIE) {
        dibujarBarraVida(x, y - 14, w * 0.8f, (float)z.vida, VIDA_MAXIMA_ZOMBIE);
    }
}

/* =====================================================================
 * Funciones publicas (las que se llaman desde main.c)
 * ===================================================================== */

void dibujarHUD(void)
{
    const char *textoOleada = TextFormat("Oleada %d / %d", rondaActual, cantidadRondas);
    int anchoOleada = MeasureText(textoOleada, 30);
    int xOleada = pxDistanciaX + anchoJardin - anchoOleada;
    int vidasRestantes = vidaCasa - zombieCasa;
    int i;

    // Titulo a la izquierda, alineado con el jardin
    DrawText("PVZ (FRUNA)", pxDistanciaX, 22, 40, RAYWHITE);

    // Oleada a la derecha
    DrawText(textoOleada, xOleada, 28, 30, RAYWHITE);

    // Vida de la casa: un corazon por cada zombie que todavia puede llegar.
    // Si Config.txt pide muchas vidas, se muestra como numero para que quepa.
    if (vidaCasa <= 10) {
        int xCorazones = xOleada - 60 - vidaCasa * 34;
        DrawText("Casa", xCorazones - MeasureText("Casa", 30) - 12, 28, 30, RAYWHITE);
        for (i = 0; i < vidaCasa; i++) {
            Color color = COLOR_CORAZON_PERDIDO;
            if (i < vidasRestantes) {
                color = RED;
            }
            dibujarCorazon((Vector2){ (float)(xCorazones + i * 34 + 14), 42 }, 11, color);
        }
    } else {
        const char *textoCasa = TextFormat("Casa: %d / %d", vidasRestantes, vidaCasa);
        DrawText(textoCasa, xOleada - 60 - MeasureText(textoCasa, 30), 28, 30, RAYWHITE);
    }
}

void dibujarContadorSoles(void)
{
    Rectangle caja = { 64, 81, 140, 50 };

    DrawRectangleRounded(caja, 0.3f, 8, COLOR_PANEL);
    dibujarIconoSol((Vector2){ caja.x + 26, caja.y + 25 }, 13);
    DrawText(TextFormat("%d", soles), (int)caja.x + 52, (int)caja.y + 10, 30, YELLOW);
}

void dibujarPanelPlantas(void)
{
    const char *nombres[7] = { "Girasol", "Lanzaguisantes", "Nuez", "Metralla", "Congeladora", "Cereza", "Pala" };
    int i;

    for (i = 0; i < 7; i++) {
        int tipo = i + 1;   // GIRASOL = 1, ..., CEREZA = 6, PALA = 7
        // Mismas medidas que usa clickMenu() en juego.c: y = 140 + 85 * i
        Rectangle carta = { 64, (float)(140 + 85 * i), 140, 75 };
        Rectangle zonaDibujo = { carta.x + 6, carta.y + 17, 56, 56 };
        int deshabilitada = 0;

        DrawRectangleRounded(carta, 0.2f, 8, COLOR_CARTA);
        textoCentrado(nombres[i], (int)(carta.x + carta.width / 2), (int)carta.y + 5, 10, RAYWHITE);
        dibujarPlanta(tipo, zonaDibujo, vidaMaxima(tipo), 1.0f);

        if (tipo == PALA) {
            DrawText("Quitar", (int)carta.x + 68, (int)carta.y + 36, 20, RAYWHITE);
        } else {
            dibujarIconoSol((Vector2){ carta.x + 80, carta.y + 46 }, 9);
            DrawText(TextFormat("%d", costoDe(tipo)), (int)carta.x + 94, (int)carta.y + 36, 20, YELLOW);
            // No alcanzan los soles, o ya hay una cereza a punto de explotar
            if ((soles < costoDe(tipo)) || ((tipo == CEREZA) && (cerezaActiva == 1))) {
                deshabilitada = 1;
            }
        }

        if (deshabilitada) {
            DrawRectangleRounded(carta, 0.2f, 8, Fade(BLACK, 0.5f));
        }
        // Borde dorado en la carta elegida
        if (plantaSeleccionada == tipo) {
            DrawRectangleLinesEx(carta, 4, GOLD);
        }
    }
}

void dibujarJardin(void)
{
    Rectangle muro = { (float)pxDistanciaX - 14, (float)pxDistanciaY, 12, (float)altoJardin };
    float yLadrillo;
    int i, j;

    // Muro de la casa: franja de ladrillos a la izquierda del jardin
    DrawRectangleRec(muro, COLOR_MURO);
    for (yLadrillo = muro.y + 18; yLadrillo < muro.y + muro.height; yLadrillo += 18) {
        DrawLineEx((Vector2){ muro.x, yLadrillo }, (Vector2){ muro.x + muro.width, yLadrillo }, 2, COLOR_MURO_OSCURO);
    }

    for (i = 0; i < filas; i++) {
        for (j = 0; j < columnas; j++) {
            // Rectangle usa float, asi no quedan espacios entre filas cuando el
            // alto de la casilla no es entero (por ejemplo 648 / 5 = 129.6)
            Rectangle casilla = rectCasilla(i, j);

            // Pasto intercalado como tablero de ajedrez
            if ((i + j) % 2 == 0) {
                DrawRectangleRec(casilla, COLOR_PASTO_CLARO);
            } else {
                DrawRectangleRec(casilla, COLOR_PASTO_OSCURO);
            }
            DrawRectangleLinesEx(casilla, 1, Fade(BLACK, 0.15f));

            // Casilla seleccionada: mas clara y con borde dorado
            if ((i == filaSeleccionada) && (j == columnaSeleccionada)) {
                DrawRectangleRec(casilla, Fade(WHITE, 0.2f));
                DrawRectangleLinesEx(casilla, 3, GOLD);
            }

            if (jardin[i][j] == OBSTACULO) {
                dibujarObstaculo(casilla);
            } else if (jardin[i][j] != VACIO) {
                int vidaMax = vidaMaxima(jardin[i][j]);
                dibujarPlanta(jardin[i][j], casilla, vidaPlanta[i][j], 1.0f);
                if ((vidaPlanta[i][j] > 0) && (vidaPlanta[i][j] < vidaMax)) {
                    dibujarBarraVida(casilla.x + 14, casilla.y + casilla.height - 14,
                                     casilla.width - 28, (float)vidaPlanta[i][j], (float)vidaMax);
                }
            }
        }
    }
}

/*
 * La cereza no se guarda en la matriz del jardin: juego.c usa cerezaActiva,
 * filaCereza, columnaCereza y tiempoCereza. Cuando explota, juego.c vuelve
 * cerezaActiva a 0 y borra la posicion.
 *
 * Para poder mostrar la explosion despues de eso, esta funcion recuerda la
 * ultima posicion de la cereza en variables 'static' (conservan su valor
 * entre una llamada y la siguiente). Es solo memoria visual: no afecta al juego.
 */
void dibujarCereza(void)
{
    static int estabaActiva = 0;
    static int ultimaFila = -1;
    static int ultimaColumna = -1;
    static double tiempoExplosion = -100;
    // Zona de dano: la casilla de la cereza y las 4 vecinas (igual que actualizarCereza)
    int desplazamientoFilas[5] = { 0, -1, 1, 0, 0 };
    int desplazamientoColumnas[5] = { 0, 0, 0, -1, 1 };
    double ahora = GetTime();
    int k;

    if (cerezaActiva == 1) {
        float progreso = (float)((ahora - tiempoCereza) / TIEMPO_CEREZA);   // de 0 a 1
        if (progreso > 1) {
            progreso = 1;
        }
        ultimaFila = filaCereza;
        ultimaColumna = columnaCereza;

        // Zona que va a explotar: se pone cada vez mas roja
        for (k = 0; k < 5; k++) {
            int f = filaCereza + desplazamientoFilas[k];
            int c = columnaCereza + desplazamientoColumnas[k];
            if ((f >= 0) && (f < filas) && (c >= 0) && (c < columnas)) {
                DrawRectangleRec(rectCasilla(f, c), Fade(RED, 0.1f + 0.25f * progreso));
            }
        }

        // La cereza se infla y parpadea a medida que se acerca la explosion
        dibujarCerezaEn(rectCasilla(filaCereza, columnaCereza), 0.85f + 0.35f * progreso, 1.0f);
        if (((int)(ahora * 12) % 2 == 0) && (progreso > 0.4f)) {
            DrawCircleV((Vector2){ pxDistanciaX + (columnaCereza + 0.5f) * anchoCasilla,
                                   pxDistanciaY + (filaCereza + 0.5f) * altoCasilla },
                        minimo(anchoCasilla, altoCasilla) * 0.4f * (0.85f + 0.35f * progreso), Fade(WHITE, 0.25f));
        }
    } else if (estabaActiva == 1) {
        // En este cuadro la cereza dejo de estar activa: acaba de explotar
        tiempoExplosion = ahora;
    }
    estabaActiva = cerezaActiva;

    // Explosion: llamarada en la zona de dano y un circulo que crece y se desvanece
    if ((ahora - tiempoExplosion < DURACION_EXPLOSION) && (ultimaFila >= 0)) {
        float t = (float)((ahora - tiempoExplosion) / DURACION_EXPLOSION);   // de 0 a 1
        float alfa = 1.0f - t;
        Vector2 centro = { pxDistanciaX + (ultimaColumna + 0.5f) * anchoCasilla,
                           pxDistanciaY + (ultimaFila + 0.5f) * altoCasilla };
        float radio = minimo(anchoCasilla, altoCasilla) * (0.5f + 1.1f * t);

        for (k = 0; k < 5; k++) {
            int f = ultimaFila + desplazamientoFilas[k];
            int c = ultimaColumna + desplazamientoColumnas[k];
            if ((f >= 0) && (f < filas) && (c >= 0) && (c < columnas)) {
                DrawRectangleRec(rectCasilla(f, c), Fade(ORANGE, 0.6f * alfa));
            }
        }
        DrawCircleV(centro, radio, Fade(ORANGE, 0.8f * alfa));
        DrawCircleV(centro, radio * 0.65f, Fade(YELLOW, 0.9f * alfa));
        DrawCircleV(centro, radio * 0.3f, Fade(WHITE, alfa));
    }
}

void dibujarVistaPrevia(void)
{
    Vector2 mouse = GetMousePosition();
    int fila, columna;
    Rectangle casilla;

    if (plantaSeleccionada == VACIO) {
        return;
    }
    // Solo si el mouse esta dentro del jardin
    if ((mouse.x < pxDistanciaX) || (mouse.x >= pxDistanciaX + anchoJardin) ||
        (mouse.y < pxDistanciaY) || (mouse.y >= pxDistanciaY + altoJardin)) {
        return;
    }
    // Misma cuenta que clickJardin() para pasar de pixeles a fila y columna
    columna = (int)((mouse.x - pxDistanciaX) / anchoCasilla);
    fila = (int)((mouse.y - pxDistanciaY) / altoCasilla);
    if ((fila < 0) || (fila >= filas) || (columna < 0) || (columna >= columnas)) {
        return;
    }
    casilla = rectCasilla(fila, columna);

    if (plantaSeleccionada == PALA) {
        // La pala solo puede quitar plantas (no obstaculos): se marca en rojo
        if ((jardin[fila][columna] != VACIO) && (jardin[fila][columna] != OBSTACULO)) {
            DrawRectangleRec(casilla, Fade(RED, 0.3f));
            DrawRectangleLinesEx(casilla, 3, RED);
        }
    } else if (jardin[fila][columna] == VACIO) {
        if ((plantaSeleccionada == CEREZA) && (cerezaActiva == 1)) {
            return;   // ya hay una cereza a punto de explotar
        }
        // La planta elegida, semitransparente, donde quedaria
        dibujarPlanta(plantaSeleccionada, casilla, vidaMaxima(plantaSeleccionada), 0.45f);
    }
}

void dibujarZombies(void)
{
    int i;
    for (i = 0; i < ZOMBIES_MAXIMOS; i++) {
        if (zombies[i].vivo == VIVO) {
            dibujarUnZombie(zombies[i], (float)anchoZombie, (float)altoZombie);
        }
    }
}

void dibujarProyectiles(void)
{
    int i;
    for (i = 0; i < PROYECTILES_MAXIMOS; i++) {
        if (proyectiles[i].vivo == VIVO) {
            Vector2 centro = { proyectiles[i].x, proyectiles[i].y };
            Color relleno = LIME;
            Color borde = DARKGREEN;

            if (proyectiles[i].tipo == METRALLA) {
                relleno = COLOR_METRALLA;
                borde = COLOR_METRALLA_OSCURO;
            } else if (proyectiles[i].tipo == CONGELADORA) {
                relleno = COLOR_HIELO;
                borde = COLOR_HIELO_OSCURO;
            }

            DrawCircleV(centro, proyectilesRadio, relleno);
            DrawCircleLines((int)centro.x, (int)centro.y, proyectilesRadio, borde);
            DrawCircleV((Vector2){ centro.x - proyectilesRadio * 0.35f, centro.y - proyectilesRadio * 0.35f },
                        proyectilesRadio * 0.25f, Fade(WHITE, 0.6f));
        }
    }
}

void dibujarSoles(void)
{
    // Los rayos giran con el tiempo: es solo un efecto visual
    float giro = (float)GetTime() * 45.0f;
    int i;

    for (i = 0; i < SOLES_MAXIMOS; i++) {
        if (solesPantalla[i] == 1) {
            Vector2 centro = { solesX[i], solesY[i] };
            DrawPoly(centro, 3, solesRadio * 1.55f, giro, Fade(ORANGE, 0.85f));
            DrawPoly(centro, 3, solesRadio * 1.55f, giro + 60, Fade(ORANGE, 0.85f));
            DrawCircleV(centro, solesRadio, GOLD);
            DrawCircleV(centro, solesRadio * 0.6f, YELLOW);
        }
    }
}

void dibujarAvisoOleada(void)
{
    double segundosRestantes = duracionGracia - (GetTime() - tiempoInicioGracia);
    const char *texto;
    int tam = 30;
    int ancho;
    Rectangle caja;

    if (rondaActiva == 1) {
        return;   // solo se muestra en el tiempo de gracia entre oleadas
    }
    if (segundosRestantes < 0) {
        segundosRestantes = 0;
    }
    texto = TextFormat("La oleada %d empieza en %d s", rondaActual, (int)segundosRestantes + 1);
    ancho = MeasureText(texto, tam);
    caja = (Rectangle){ pxDistanciaX + anchoJardin / 2.0f - ancho / 2.0f - 20, pxDistanciaY + 16.0f,
                        (float)(ancho + 40), (float)(tam + 20) };

    DrawRectangleRounded(caja, 0.4f, 8, Fade(BLACK, 0.6f));
    DrawText(texto, (int)caja.x + 20, (int)caja.y + 10, tam, RAYWHITE);
}

void dibujarAyuda(void)
{
    textoCentrado("Elige una carta y haz click en una casilla.   Recoge los soles con un click.   La pala quita plantas.",
                  pxAncho / 2, pxAlto - 55, 20, Fade(RAYWHITE, 0.8f));
}

void dibujarCursorPala(void)
{
    Vector2 mouse = GetMousePosition();
    if (plantaSeleccionada == PALA) {
        dibujarPala((Rectangle){ mouse.x + 4, mouse.y - 64, 64, 64 }, 1.0f);
    }
}

Rectangle botonMenu(int indice)
{
    Rectangle boton;
    boton.width = pxAncho / 3.0f;
    boton.height = pxAlto / 7.0f;
    boton.x = (pxAncho - boton.width) / 2;                         // centrado
    boton.y = pxAlto * 0.32f + indice * (boton.height + 25);       // uno debajo del otro
    return boton;
}

void dibujarMenu(void)
{
    const char *textos[3] = { "JUGAR", "CONFIGURACIÓN", "SALIR" };
    Vector2 mouse = GetMousePosition();
    int yTitulo = (int)(pxAlto * 0.08f);
    int i;

    textoCentrado("PVZ (FRUNA)", pxAncho / 2, yTitulo, 80, RAYWHITE);
    textoCentrado("Plantas vs Zombies", pxAncho / 2, yTitulo + 90, 30, COLOR_PASTO_CLARO);

    for (i = 0; i < 3; i++) {
        Rectangle boton = botonMenu(i);
        Color color = COLOR_BOTON;

        // El boton se aclara cuando el mouse esta encima
        if (CheckCollisionPointRec(mouse, boton)) {
            color = COLOR_BOTON_ENCIMA;
        }
        DrawRectangleRounded(boton, 0.35f, 10, color);
        textoCentrado(textos[i], (int)(boton.x + boton.width / 2), (int)(boton.y + boton.height / 2) - 20, 40, BLACK);
    }
}

void dibujarPantallaFinal(int gano)
{
    float tam = 230;   // tamano de los dibujos de los costados
    Rectangle izquierda = { pxAncho / 2.0f - 560, pxAlto / 2.0f - tam / 2 - 20, tam, tam };
    Rectangle derecha = { pxAncho / 2.0f + 560 - tam, pxAlto / 2.0f - tam / 2 - 20, tam, tam };

    if (gano) {
        dibujarGirasol(izquierda, 1.0f);
        dibujarGirasol(derecha, 1.0f);
        textoCentrado("¡GANASTE!", pxAncho / 2, pxAlto / 2 - 100, 100, GOLD);
        textoCentrado("Sobreviviste a todas las oleadas", pxAncho / 2, pxAlto / 2 + 20, 30, RAYWHITE);
    } else {
        Zombie decoracion = { 0 };
        decoracion.tipo = NORMAL;
        decoracion.estado = ATACANDO;
        decoracion.vida = VIDA_MAXIMA_ZOMBIE;
        decoracion.x = izquierda.x + tam * 0.3f;
        decoracion.y = izquierda.y;
        dibujarUnZombie(decoracion, tam * 0.45f, tam * 0.75f);
        decoracion.x = derecha.x + tam * 0.3f;
        dibujarUnZombie(decoracion, tam * 0.45f, tam * 0.75f);
        textoCentrado("PERDISTE", pxAncho / 2, pxAlto / 2 - 100, 100, RED);
        textoCentrado("Los zombies llegaron a tu casa", pxAncho / 2, pxAlto / 2 + 20, 30, RAYWHITE);
    }
    textoCentrado("Presiona ESC para salir", pxAncho / 2, pxAlto / 2 + 80, 20, Fade(RAYWHITE, 0.7f));
}
