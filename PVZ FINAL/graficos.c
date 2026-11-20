#include "raylib.h"
#include "juego.h"
#include "graficos.h"

Texture2D texturaSol;
Texture2D texturaCorazon;
Texture2D texturaCorazonNegro;
Texture2D texturaObstaculo;
Texture2D texturaGirasol;
Texture2D texturaLanzaguisantes;
Texture2D texturaNuez;
Texture2D texturaMetralla;
Texture2D texturaCongeladora;
Texture2D texturaCereza;
Texture2D texturaPala;
Texture2D texturaZombieNormal;
Texture2D texturaZombieFlag;
Texture2D texturaZombieBalde;
Texture2D texturaGuisante;
Texture2D texturaGuisanteHelado;

// Colores personalizados para el juego
#define COLOR_PASTO_CLARO (Color){112, 178, 68, 255}
#define COLOR_PASTO_OSCURO (Color){94, 158, 54, 255}
#define COLOR_PIEL_CONGELADA (Color){160, 210, 235, 255}
#define COLOR_MURO (Color){150, 140, 130, 255}
#define COLOR_MURO_OSCURO (Color){100, 90, 85, 255}
#define COLOR_PANEL (Color){55, 55, 60, 255}
#define COLOR_CARTA (Color){40, 75, 125, 255}
#define COLOR_BOTON (Color){130, 130, 130, 255}
#define COLOR_BOTON_ENCIMA (Color){90, 90, 90, 255}



//Funcion para cargar las texturas utilizadas en el juego
void cargarTexturas(void) {
    texturaSol = LoadTexture("imagenes/varios/Sol.png");
    texturaCorazon = LoadTexture("imagenes/varios/Corazon.png");
    texturaCorazonNegro = LoadTexture("imagenes/varios/Corazon_negro.png");
    texturaObstaculo = LoadTexture("imagenes/varios/Tumbas.png");

    texturaGirasol = LoadTexture("imagenes/plantas/Girasol.png");
    texturaLanzaguisantes = LoadTexture("imagenes/plantas/Lanzaguisantes.png");
    texturaNuez = LoadTexture("imagenes/plantas/Nuez.png");
    texturaMetralla = LoadTexture("imagenes/plantas/Metralla.png");
    texturaCongeladora = LoadTexture("imagenes/plantas/Congeladora.png");
    texturaCereza = LoadTexture("imagenes/plantas/Cereza.png");
    texturaPala = LoadTexture("imagenes/plantas/Pala.png");

    texturaZombieNormal = LoadTexture("imagenes/zombies/Normal.png");
    texturaZombieFlag = LoadTexture("imagenes/zombies/Flag.png");
    texturaZombieBalde = LoadTexture("imagenes/zombies/Balde.png");

    texturaGuisante = LoadTexture("imagenes/proyectiles/Guisante.png");
    texturaGuisanteHelado = LoadTexture("imagenes/proyectiles/Guisante_helado.png");
}



//Funcion para liberar la memoria que ocupan las texturas
void liberarTexturas(void) {
    UnloadTexture(texturaSol);
    UnloadTexture(texturaCorazon);
    UnloadTexture(texturaCorazonNegro);
    UnloadTexture(texturaObstaculo);

    UnloadTexture(texturaGirasol);
    UnloadTexture(texturaLanzaguisantes);
    UnloadTexture(texturaNuez);
    UnloadTexture(texturaMetralla);
    UnloadTexture(texturaCongeladora);
    UnloadTexture(texturaCereza);
    UnloadTexture(texturaPala);

    UnloadTexture(texturaZombieNormal);
    UnloadTexture(texturaZombieFlag);
    UnloadTexture(texturaZombieBalde);

    UnloadTexture(texturaGuisante);
    UnloadTexture(texturaGuisanteHelado);
}



//Funcion para dibujar una textura adaptandose al tamaño del rectangulo de destino (casilla/menu)
void dibujarTextura(Texture2D textura, Rectangle destino, float transparencia, Color tinte) {
    if (textura.id == 0) { //Primero comprueba que la textura se haya cargado correctamente
        return;
    }
    float escala = minimo(destino.width / textura.width, destino.height / textura.height);
    float ancho = textura.width * escala;
    float alto = textura.height * escala;

    Rectangle ajustado = {destino.x + (destino.width - ancho) / 2, destino.y + (destino.height - alto) / 2, ancho, alto};
    DrawTexturePro(textura, (Rectangle){0, 0, textura.width, textura.height}, ajustado, (Vector2){0, 0}, 0, Fade(tinte, transparencia));
}



//Funcion para asignar la textura a la planta llamada
Texture2D texturaPlanta(int tipo) {
    if (tipo == GIRASOL) 
        return texturaGirasol;
    if (tipo == LANZAGUISANTES) 
        return texturaLanzaguisantes;
    if (tipo == NUEZ) 
        return texturaNuez;
    if (tipo == METRALLA) 
        return texturaMetralla;
    if (tipo == CONGELADORA) 
        return texturaCongeladora;
    if (tipo == CEREZA) 
        return texturaCereza;
    if (tipo == PALA) 
        return texturaPala;
    return (Texture2D){ 0 }; //En caso de que se llame un valor invalido devuelve una textura vacia (Raylib)
}



// Escribe un texto centrado horizontalmente en centroX
void textoCentrado(const char *texto, int centroX, int y, int tam, Color color) {
    int ancho = MeasureText(texto, tam); //Cuantos pixeles de ancho ocupa el texto
    DrawText(texto, centroX - (ancho / 2), y, tam, color);
}



//Calcula el minimo entre dos valores (para no incluir <math.h>)
float minimo(float a, float b) {
    if (a < b) {
        return a;
    }
    return b;
}



//Da el rectangulo (casilla) correspondiente del jardin
Rectangle rectCasilla(int fila, int columna) {
    Rectangle rect = { pxDistanciaX + columna * anchoCasilla, pxDistanciaY + fila * altoCasilla, anchoCasilla, altoCasilla };
    return rect;
}



// Barra de vida con fondo oscuro y relleno verde, amarillo o rojo segun su vida
void dibujarBarraVida(float x, float y, float ancho, float vida, float vidaMax) {
    if (vidaMax <= 0) { //Primero verifica que la vida sea mayor que 0
        return;
    }
    float porcentajeVida = vida / vidaMax;
    Color color;

    //Nos aseguramos que su vida este dentro del rango [0-1]
    if (porcentajeVida < 0) {
        porcentajeVida = 0;
    }
    if (porcentajeVida > 1) {
        porcentajeVida = 1;
    }
    //Asignamos su color
    if (porcentajeVida > 0.666) {
        color = LIME;
    } else if (porcentajeVida > 0.333) {
        color = GOLD;
    } else {
        color = RED;
    }

    DrawRectangleRec((Rectangle){ x, y, ancho, 7 }, Fade(BLACK, 0.50)); //Fade es para la opacidad
    DrawRectangleRec((Rectangle){ x + 1, y + 1, (ancho - 2) * porcentajeVida, 5 }, color);
}



//Para dibujar las vidas que tiene el jugador (con textura si tiene la vida o la perdio)
void dibujarCorazon(Vector2 c, float r, int perdido) {
    Rectangle destino = {c.x - r, c.y - r, r * 2, r * 2};
    if (perdido) {
        dibujarTextura(texturaCorazonNegro, destino, 1.0, WHITE);
    } else {
        dibujarTextura(texturaCorazon, destino, 1.0, WHITE);
    }
}



// Dibuja un sol para la interfaz utilizando su textura
void dibujarIconoSol(Vector2 centro, float radio) {
    Rectangle destino = {centro.x - radio, centro.y - radio, radio * 2, radio * 2};
    dibujarTextura(texturaSol, destino, 1.0, WHITE);
}



//Funcion para dibujar la cereza, ajustando su tamaño y transparencia
void dibujarCerezaEn(Rectangle r, float escala, float alfa) {
    float ancho = r.width * escala;
    float alto = r.height * escala;
    Rectangle destino = {r.x + (r.width - ancho) / 2, r.y + (r.height - alto) / 2, ancho, alto};
    dibujarTextura(texturaCereza, destino, alfa, WHITE);
}



//Funcion para dibujar la textura de la pala
void dibujarPala(Rectangle r, float alfa) {
    dibujarTextura(texturaPala, r, alfa, WHITE);
}



// Dibuja el obstaculo reducido y centrado dentro de su casilla
void dibujarObstaculo(Rectangle r) {
    float ancho = r.width * 0.85;
    float alto = r.height * 0.85;
    Rectangle destino = {r.x + (r.width - ancho) / 2, r.y + (r.height - alto) / 2, ancho, alto};
    dibujarTextura(texturaObstaculo, destino, 1.0, WHITE);
}



//Dibuja la planta correspodiente
void dibujarPlanta(int tipo, Rectangle r, float alfa) {
    Texture2D textura = texturaPlanta(tipo);
    dibujarTextura(textura, r, alfa, WHITE);
}



//Funcion para dibujar un zombie
void dibujarUnZombie(Zombie z, float w, float h) {
    float x = z.x;
    float y = z.y;

    // Seleccionamos la textura segun el tipo de zombie
    Texture2D textura;

    if (z.tipo == FLAG) {
        textura = texturaZombieFlag;
    } else if (z.tipo == BALDE) {
        textura = texturaZombieBalde;
    } else {
        textura = texturaZombieNormal;
    }

    // Calculamos el espacio donde se dibujara el zombie
    Rectangle destino = {x, y, w, h};

    // Si el zombie esta congelado se pinta de azul, si no, se dibuja normal
    if (z.congelado == CONGELADO) {
        dibujarTextura(textura, destino, 1.0, COLOR_PIEL_CONGELADA);
    } else {
        dibujarTextura(textura, destino, 1.0, WHITE);
    }

    //Barra de vida arriba del zombie, solo si ya recibio dano
    if (z.vida < vidaMaximaZombie(z.tipo)) {
        dibujarBarraVida(x, y - 14, w * 0.8, (float)z.vida, vidaMaximaZombie(z.tipo));
    }
}



//Funcion para dibujar todo el HUD
void dibujarHUD(void) {

    //Prepara el texto de la oleada actual
    const char *textoOleada = TextFormat("Oleada %d / %d", rondaActual, cantidadRondas);
    int anchoOleada = MeasureText(textoOleada, 30);
    int xOleada = pxDistanciaX + anchoJardin - anchoOleada;

    //Calcula las vidas que le quedan a la casa
    int vidasRestantes = vidaCasa - zombieCasa;

    DrawText("Plantas vs Zombies", pxDistanciaX, 22, 40, RAYWHITE); //Titulo a la izquierda
    DrawText(textoOleada, xOleada, 28, 30, RAYWHITE); //Oleada actual a la derecha

    // Un corazon por cada vida (si Config.txt pide muchas vidas se muestra como numero para que quepa)
    if (vidaCasa <= 10) {
        //Calcula la posicion en el eje x de la casa
        int xCorazones = xOleada - 60 - vidaCasa * 34;
        DrawText("Casa", xCorazones - MeasureText("Casa", 30) - 12, 28, 30, RAYWHITE);
        for (int i = 0; i < vidaCasa; i++) {
            int perdido = (i >= vidasRestantes); //Determina si el corazon es vida perdida o no
            dibujarCorazon((Vector2){ (float)(xCorazones + i * 34 + 14), 42 }, 11, perdido);
        }
    } else {
        const char *textoCasa = TextFormat("Casa: %d / %d", vidasRestantes, vidaCasa);
        DrawText(textoCasa, xOleada - 60 - MeasureText(textoCasa, 30), 28, 30, RAYWHITE);
    }
}



//Funcion para el contador de soles
void dibujarContadorSoles(void) {
    Rectangle caja = { 64, 81, 140, 50 };
    DrawRectangleRounded(caja, 0.3, 8, COLOR_PANEL);
    dibujarIconoSol((Vector2){ caja.x + 26, caja.y + 25 }, 13);
    DrawText(TextFormat("%d", soles), (int)caja.x + 52, (int)caja.y + 10, 30, YELLOW);
}



//Funcion para dibujar el menu de la plantas
void dibujarMenuPlantas(void) {
    //Nombre de las plantas en el orden del menu
    const char *nombres[7] = {"Girasol", "Lanzaguisantes", "Nuez", "Metralla", "Congeladora", "Cereza", "Pala"};

    //Recorre cada planta del menu
    for (int i = 0; i < 7; i++) {
        int tipo = i + 1;
        Rectangle carta = {64, (float)(140 + 85 * i), 140, 75};
        Rectangle zonaDibujo = {carta.x + 6, carta.y + 17, 56, 56};
        int deshabilitada = 0;

        DrawRectangleRounded(carta, 0.2, 8, COLOR_CARTA);
        textoCentrado(nombres[i], (int)(carta.x + carta.width / 2), (int)carta.y + 5, 10, RAYWHITE);
        dibujarPlanta(tipo, zonaDibujo, 1.0);

        //Todas las plantas muestran su costo, excepto la pala que muestra un texto
        if (tipo == PALA) {
            DrawText("Quitar", (int)carta.x + 68, (int)carta.y + 36, 20, RAYWHITE);
        } else {
            dibujarIconoSol((Vector2){ carta.x + 80, carta.y + 46 }, 9);
            DrawText(TextFormat("%d", costoPlanta(tipo)), (int)carta.x + 94, (int)carta.y + 36, 20, YELLOW);
            //Comprueba si la planta no debe poder seleccionarse
            if ((soles < costoPlanta(tipo)) || ((tipo == CEREZA) && (cerezaActiva == 1))) {
                deshabilitada = 1;
            }
        }

        if (deshabilitada == 1) {
            DrawRectangleRounded(carta, 0.2, 8, Fade(BLACK, 0.5));
        }
        //Borde a la planta elegida
        if (plantaSeleccionada == tipo) {
            DrawRectangleLinesEx(carta, 4, GOLD);
        }
    }
}



//Funcion para dibujar el jardin
void dibujarJardin(void) {
    Rectangle muro = { (float)pxDistanciaX - 14, (float)pxDistanciaY, 12, (float)altoJardin };
    float yLadrillo;

    // Muro de la casa a la izquierda del jardin
    DrawRectangleRec(muro, COLOR_MURO);
    for (yLadrillo = muro.y + 18; yLadrillo < muro.y + muro.height; yLadrillo += 18) {
        DrawLineEx((Vector2){ muro.x, yLadrillo }, (Vector2){ muro.x + muro.width, yLadrillo }, 2, COLOR_MURO_OSCURO);
    }

    //Recorre todas las casillas del jardín
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            Rectangle casilla = rectCasilla(i, j);

            //Pasto intercalado de color
            if ((i + j) % 2 == 0) {
                DrawRectangleRec(casilla, COLOR_PASTO_CLARO);
            } else {
                DrawRectangleRec(casilla, COLOR_PASTO_OSCURO);
            }
            DrawRectangleLinesEx(casilla, 1, Fade(BLACK, 0.15));

            //Hace la casilla seleccionada mas clara y con borde resaltado
            if ((i == filaSeleccionada) && (j == columnaSeleccionada)) {
                DrawRectangleRec(casilla, Fade(WHITE, 0.2));
                DrawRectangleLinesEx(casilla, 3, GOLD);
            }

            //Dibuja un obstaculo si lo hay, y verifica si la planta ha sufrido daño para mostar su barra de vida
            if (jardin[i][j] == OBSTACULO) {
                dibujarObstaculo(casilla);
            } else if (jardin[i][j] != VACIO) {
                // Calcula el tamaño de la planta y la centra dentro de la casilla
                float anchoPlanta = casilla.width * 0.75;
                float altoPlanta = casilla.height * 0.75;
                Rectangle destino = {casilla.x + (casilla.width - anchoPlanta) / 2, casilla.y + (casilla.height - altoPlanta) / 2, anchoPlanta, altoPlanta};
                dibujarPlanta(jardin[i][j], destino, 1.0);

                int vidaMax = vidaMaxima(jardin[i][j]);
                if ((vidaPlanta[i][j] > 0) && (vidaPlanta[i][j] < vidaMax)) {
                    dibujarBarraVida(destino.x, destino.y + destino.height - 8, destino.width, (float)vidaPlanta[i][j], (float)vidaMax);
                }
            }
        }
    }
}



/*
La cereza no se guarda en la matriz del jardin, para poder mostrar la explosion despues de 
eso, esta funcion recuerda la ultima posicion. Es solo memoria visual: no afecta al juego.
*/
void dibujarCereza(void) {
    //Se declaran las variables con static para que conserven su valor entre llamadas
    static int estabaActiva = 0;
    static int ultimaFila = -1;
    static int ultimaColumna = -1;
    static double tiempoExplosion = -100;

    //Movimiento para identificar la casilla central y las de alrededor
    int desplazamientoFilas[5] = { 0, -1, 1, 0, 0 };
    int desplazamientoColumnas[5] = { 0, 0, 0, -1, 1 };
    double ahora = GetTime();

    if (cerezaActiva == 1) {
        //Calcula el progreso de preparacion de la cereza (de 0 a 1)
        float progreso = (float)((ahora - tiempoCereza) / TIEMPO_CEREZA);
        if (progreso > 1) {
            progreso = 1;
        }
        //Guarda la posicion de la cereza para usarla en la explosion
        ultimaFila = filaCereza;
        ultimaColumna = columnaCereza;

        //La zona que va a explotar se pone mas roja
        for (int i = 0; i < 5; i++) {
            int f = filaCereza + desplazamientoFilas[i];
            int c = columnaCereza + desplazamientoColumnas[i];

            if ((f >= 0) && (f < filas) && (c >= 0) && (c < columnas)) { //Comprueba que este dentro del jardin
                DrawRectangleRec(rectCasilla(f, c), Fade(RED, 0.1 + 0.25 * progreso));
            }
        }

        //La cereza se agranda a medida que se acerca la explosion
        dibujarCerezaEn(rectCasilla(filaCereza, columnaCereza), 0.85 + 0.35 * progreso, 1.0);
        if (((int)(ahora * 12) % 2 == 0) && (progreso > 0.4)) { //Hace parpadear la cereza cuando esta cerca de explotar
            DrawCircleV((Vector2){ pxDistanciaX + (columnaCereza + 0.5) * anchoCasilla, pxDistanciaY + (filaCereza + 0.5) * altoCasilla },
            minimo(anchoCasilla, altoCasilla) * 0.4 * (0.85 + 0.35 * progreso), Fade(WHITE, 0.25));
        }
    } else if (estabaActiva == 1) {
        //Detecta que la cereza exploto y guarda el momento
        tiempoExplosion = ahora;
    }
    estabaActiva = cerezaActiva;


    //Comprueba si todavia tiene que mostrarse la animacion de explosion
    if ((ahora - tiempoExplosion < DURACION_EXPLOSION) && (ultimaFila >= 0)) {
        float t = (float)((ahora - tiempoExplosion) / DURACION_EXPLOSION); // de 0 a 1
        float alfa = 1.0 - t;
        //Calcula el centro y el radio de la explosion
        Vector2 centro = {pxDistanciaX + (ultimaColumna + 0.5) * anchoCasilla, pxDistanciaY + (ultimaFila + 0.5) * altoCasilla};
        float radio = minimo(anchoCasilla, altoCasilla) * (0.5 + 1.1 * t);

        //Dibuja en rojo las casillas afectadas
        for (int i = 0; i < 5; i++) {
            int f = ultimaFila + desplazamientoFilas[i];
            int c = ultimaColumna + desplazamientoColumnas[i];
            if ((f >= 0) && (f < filas) && (c >= 0) && (c < columnas)) {
                DrawRectangleRec(rectCasilla(f, c), Fade(ORANGE, 0.6 * alfa));
            }
        }

        // Dibuja circulos de diferentes colores para representar la explosion
        DrawCircleV(centro, radio, Fade(ORANGE, 0.8 * alfa));
        DrawCircleV(centro, radio * 0.65, Fade(YELLOW, 0.9 * alfa));
        DrawCircleV(centro, radio * 0.3, Fade(WHITE, alfa));
    }
}



//Funcion para dibujar la vista previa de la planta seleccionada bajo el cursor
void dibujarVistaPrevia(void) {
    Vector2 mouse = GetMousePosition();
    int fila, columna;
    Rectangle casilla;

    // No muestra ninguna vista previa si no hay una planta seleccionada
    if (plantaSeleccionada == VACIO) {
        return;
    }

    // Comprueba que el mouse se encuentre dentro de los limites del jardin
    if ((mouse.x < pxDistanciaX) || (mouse.x >= pxDistanciaX + anchoJardin) || (mouse.y < pxDistanciaY) || (mouse.y >= pxDistanciaY + altoJardin)) {
        return;
    }

    // Convierte las coordenadas del mouse en indices de fila y columna
    columna = (int)((mouse.x - pxDistanciaX) / anchoCasilla);
    fila = (int)((mouse.y - pxDistanciaY) / altoCasilla);
    casilla = rectCasilla(fila, columna);

    if (plantaSeleccionada == PALA) {
        // Resalta en rojo las casillas que contienen una planta que puede quitarse
        if ((jardin[fila][columna] != VACIO) && (jardin[fila][columna] != OBSTACULO)) {
            DrawRectangleRec(casilla, Fade(RED, 0.3));
            DrawRectangleLinesEx(casilla, 3, RED);
        }
    } else if (jardin[fila][columna] == VACIO) {
        // No muestra la vista previa si ya hay otra cereza activa
        if ((plantaSeleccionada == CEREZA) && (cerezaActiva == 1)) {
            return;
        }

        // Calcula el tamaño de la planta y la centra dentro de la casilla
            float anchoPlanta = casilla.width * 0.75;
            float altoPlanta = casilla.height * 0.75;
            Rectangle destino = {casilla.x + (casilla.width - anchoPlanta) / 2, casilla.y + (casilla.height - altoPlanta) / 2, anchoPlanta, altoPlanta};
            // Dibuja la vista previa de la planta de forma semitransparente
            dibujarPlanta(plantaSeleccionada, destino, 0.45);
    }
}



// Funcion para dibujar todos los zombies activos
void dibujarZombies(void) {
    Vector2 tamano = obtenerTamanoZombie();
    for (int i = 0; i < ZOMBIES_MAXIMOS; i++) {
        // Dibuja unicamente los zombies que siguen vivos
        if (zombies[i].vivo == VIVO) {
            dibujarUnZombie(zombies[i], tamano.x, tamano.y);
        }
    }
}



//Funcion para dibujar todos los proyectiles activos
void dibujarProyectiles(void) {
    for (int i = 0; i < PROYECTILES_MAXIMOS; i++) {
        if (proyectiles[i].vivo == VIVO) {
            // Utiliza la textura normal por defecto
            Texture2D textura = texturaGuisante;

            // Selecciona la textura correspondiente si el proyectil es congelante
            if (proyectiles[i].tipo == CONGELADORA) {
                textura = texturaGuisanteHelado;
            }

            Rectangle destino = {proyectiles[i].x - proyectilesRadio, proyectiles[i].y - proyectilesRadio, proyectilesRadio * 2, proyectilesRadio * 2};
            dibujarTextura(textura, destino, 1.0, WHITE); // Dibuja el proyectil utilizando la textura seleccionada
        }
    }
}



// Funcion para dibujar todos los soles disponibles en pantalla
void dibujarSoles(void) {
    for (int i = 0; i < SOLES_MAXIMOS; i++) {
        if (solesPantalla[i] == 1) {
            Rectangle destino = {solesX[i] - solesRadio, solesY[i] - solesRadio, solesRadio * 2, solesRadio * 2};
            dibujarTextura(texturaSol, destino, 1.0, WHITE);
        }
    }
}



//Funcion para dibujar el aviso de inicio de una nueva oleada
void dibujarAvisoOleada(void) {
    // Calcula cuantos segundos faltan para que termine el tiempo de gracia
    double segundosRestantes = duracionGracia - (GetTime() - tiempoInicioGracia);
    const char *texto;
    int tam = 30;
    int ancho;
    Rectangle caja;

    // No muestra el aviso mientras una oleada esta activa
    if (rondaActiva == 1) {
        return;
    }
    if (segundosRestantes < 0) { // Evita que el tiempo restante sea negativo
        segundosRestantes = 0;
    }

    // Prepara el texto del aviso y calcula su ancho en pantalla
    texto = TextFormat("La oleada %d empieza en %d s", rondaActual, (int)segundosRestantes + 1);
    ancho = MeasureText(texto, tam);

    // Dibuja el fondo semitransparente y el texto del aviso
    caja = (Rectangle){ pxDistanciaX + anchoJardin / 2.0 - ancho / 2.0 - 20, pxDistanciaY + 16.0, (float)(ancho + 40), (float)(tam + 20) };
    DrawRectangleRounded(caja, 0.4, 8, Fade(BLACK, 0.6));
    DrawText(texto, (int)caja.x + 20, (int)caja.y + 10, tam, RAYWHITE);
}



// Funcion para dibujar la pala al lado del mouse cuando se selecciona
void dibujarCursorPala(void) {
    //Obtiene la posicion actual del mouse
    Vector2 mouse = GetMousePosition();
    if (plantaSeleccionada == PALA) {
        dibujarPala((Rectangle){mouse.x + 4, mouse.y - 64, 64, 64}, 1.0);
    }
}



// Funcion para calcular la posicion y el tamano de un boton del menu
Rectangle botonMenu(int indice) {
    Rectangle boton;
    //Define las dimensiones del boton
    boton.width = pxAncho / 3.0;
    boton.height = pxAlto / 7.0;
    boton.x = (pxAncho - boton.width) / 2; //Centra horizontalmente el boton
    boton.y = pxAlto * 0.32 + indice * (boton.height + 25); //Posiciona cada boton debajo del anterior
    return boton;
}



// Funcion para dibujar el menu principal del juego
void dibujarMenu(void) {
    // Textos correspondientes a las opciones del menu
    const char *textos[2] = {"JUGAR", "SALIR"};

    Vector2 mouse = GetMousePosition();
    int yTitulo = (int)(pxAlto * 0.08);

    //Dibuja el titulo principal y el subtitulo
    textoCentrado("PVZ", pxAncho / 2, yTitulo, 80, RAYWHITE);
    textoCentrado("Plantas vs Zombies", pxAncho / 2, yTitulo + 90, 30, COLOR_PASTO_CLARO);

    //Recorre las dos opciones del menu
    for (int i = 0; i < 2; i++) {
        Rectangle boton = botonMenu(i);
        Color color = COLOR_BOTON;
        // Cambia el color del boton cuando el cursor se encuentra encima
        if (CheckCollisionPointRec(mouse, boton)) {
            color = COLOR_BOTON_ENCIMA;
        }

        //Dibuja el boton y centra su texto
        DrawRectangleRounded(boton, 0.35, 10, color);
        textoCentrado(textos[i], (int)(boton.x + boton.width / 2), (int)(boton.y + boton.height / 2) - 40, 80, BLACK);
    }
}



// Funcion para dibujar la pantalla de victoria o derrota
void dibujarPantallaFinal(int gano) {
    // Define el tamano y la posicion de los dibujos decorativos laterales
    float tam = 230;
    Rectangle izquierda = { pxAncho / 2.0 - 560, pxAlto / 2.0 - tam / 2 - 20, tam, tam };
    Rectangle derecha = { pxAncho / 2.0 + 560 - tam, pxAlto / 2.0 - tam / 2 - 20, tam, tam };

    if (gano) {
        // Dibuja girasoles a ambos lados de la pantalla
        dibujarTextura(texturaGirasol, izquierda, 1.0, WHITE);
        dibujarTextura(texturaGirasol, derecha, 1.0, WHITE);

        // Muestra el mensaje de victoria
        textoCentrado("¡GANASTE!", pxAncho / 2, pxAlto / 2 - 100, 100, GOLD);
        textoCentrado("Sobreviviste a todas las oleadas", pxAncho / 2, pxAlto / 2 + 20, 30, RAYWHITE);
    } else {
        // Crea un zombie temporal para utilizarlo como elemento decorativo
        Zombie decoracion = { 0 };

        // Configura los datos necesarios para dibujar el zombie
        decoracion.tipo = NORMAL;
        decoracion.estado = ATACANDO;

        // Dibuja un zombie a cada lado de la pantalla
        decoracion.x = izquierda.x + tam * 0.3;
        decoracion.y = izquierda.y;
        dibujarUnZombie(decoracion, tam * 0.45, tam * 0.75);

        decoracion.x = derecha.x + tam * 0.3;
        dibujarUnZombie(decoracion, tam * 0.45, tam * 0.75);

        // Muestra el mensaje de derrota
        textoCentrado("PERDISTE", pxAncho / 2, pxAlto / 2 - 100, 100, RED);
        textoCentrado("Los zombies llegaron a tu casa", pxAncho / 2, pxAlto / 2 + 20, 30, RAYWHITE);
    }

    // Muestra la instruccion para salir del juego
    textoCentrado("Presiona ESC para salir", pxAncho / 2, pxAlto / 2 + 80, 20, Fade(RAYWHITE, 0.7));
}