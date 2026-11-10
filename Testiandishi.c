#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

//Estados del juego
#define MENU 0
#define JUGANDO 1
#define WIN 2
#define LOSE 3

//Definiciones para la matriz del jardin
#define VACIO 0
#define GIRASOL 1
#define LANZAGUISANTES 2
#define NUEZ 3

//Definir vida de las plantas
#define VIDA_GIRASOL 40
#define VIDA_LANZAGUISANTES 40
#define VIDA_NUEZ 200

//Para diferenciar la config de los soles de los girasoles y los del cielo
#define SOLES_MAXIMOS 50 //Cantidad maxima de soles en pantalla
#define SOL_GIRASOL 1
#define SOL_CIELO 2

// Definiciones para los tipos de zombies
#define ZOMBIES_MAXIMOS 50 //Cantidad maxima de zombies
#define NORMAL 1
#define FLAG 2

// Estado del zombie
#define CAMINANDO 1
#define ATACANDO 2


#define PROYECTILES_MAXIMOS 100
// Esto es para los proyectiles y los zombies
#define MUERTO 0
#define VIVO 1

//Aqui estan todas las funciones de raylib: https://www.raylib.com/cheatsheet/cheatsheet.html

//Struct con las stats de los zombies
typedef struct {
    float x;
    float y;
    int vida;
    float velocidad;
    int vivo;
    int tipo;
    int estado;
    int columnaAtaque;
    int filaAtaque;
    int daño;
    float cooldownAtaque;
    double tiempoUltimoAtaque;
} Zombie; 

//Struct con las stats de los proyectiles
typedef struct {
    float x;
    float y;
    int daño;
    float velocidad;
    int tipo;
    int vivo;
} Proyectil;

int main(void) {
    //Estado que inicia el juego
    int estadoJuego = MENU;

    // Ajustamos el tamaño del patio
    int filas = 5;
    int columnas = 9;

    // Duracion del juego
    int vidaCasa = 3;
    int zombieCasa = 0;
    int cantidadRondas = 2;
    double duracionGracia = 5.0;
    double tiempoInicioGracia = GetTime(); 
    int rondaActual = 1;
    int rondaActiva = 0; // 0 = no hay ronda activa, 1 = si hay ronda activa

    // Dimensiones de la ventana donde se abre el juego
    int pxAncho = 1440; //1440
    int pxAlto = 810; // 810

    // Dimensiones del jardin
    int pxDistanciaX = 224; //Distancia de izq a dd empieza el jardin
    int pxDistanciaY = 81; //Distancia de arriba a dd empieza el jardin
    int anchoJardin = 1152; //Ancho total del jardin
    int altoJardin = 648; //Alto total del jardin

    // Soles iniciales y costos
    int soles = 1000;
    int costoGirasol = 50;
    int costoLanzaguisantes = 100;
    int costoNuez = 50;

    // Variables para los soles que spawnean
    float solesRadio = 17.0;
    float solesX[SOLES_MAXIMOS]; //Coords eje x
    float solesY[SOLES_MAXIMOS]; //Coords eje y
    int solesTipo[SOLES_MAXIMOS]; //Cielo o girasol
    int solesPantalla[SOLES_MAXIMOS]; //Valor booleano de si en las coords hay un sol
    double solesTiempo[SOLES_MAXIMOS]; //Cuanto dura un sol antes de despawnear
    for (int i = 0; i < SOLES_MAXIMOS; i++) {
        solesX[i] = 0;
        solesY[i] = 0;
        solesTipo[i] = 0;
        solesPantalla[i] = 0;
        solesTiempo[i] = 0;
    }
    double cooldownSol = 7.0;
    double duracionSol = 4.5;
    float velocidadSol = 40.0; //Como bajan del cielo
    double tiempoUltimoSolCielo = 0;
    double cooldownSolCielo = 10.0;  

    //Variables para los zombies
    double tiempoUltimoZombie = 0;
    float cooldownZombie = 5.0;
    int zombiesGenerados = 0;
    int zombiesPorRonda = 10;
    int zombiesVivos = 0;
    //Probabilidad de cada zombie
    float probabilidadNormal = 80;

    //Proyectiles
    float cooldownProyectil = 2.0;
    float proyectilesRadio = 13.0;

    Zombie zombies[ZOMBIES_MAXIMOS]; //Declaramos zombies como la funcion que tiene todas las stats de los zombies
    for (int i = 0; i < ZOMBIES_MAXIMOS; i++) { //Inicializamos todos los zombies como muertos
        zombies[i].vivo = MUERTO;
    }

    Proyectil proyectiles[PROYECTILES_MAXIMOS]; //Declaramos proyectiles como la funcion que tiene todas las stats de los proyectiles
    for (int i = 0; i < PROYECTILES_MAXIMOS; i++) { //Inicializamos todos los proyectiles como muertos
        proyectiles[i].vivo = MUERTO;
    }





    /*
    Reserva la memoria dinamica para cada 'matriz' (en filas primero)
    */
    int **jardin = malloc(filas * sizeof(int *));
    double **tiempoUltimoSolGirasol = malloc(filas * sizeof(double *));
    double **tiempoUltimoDisparo = malloc(filas * sizeof(double *));
    float **vidaPlanta = malloc(filas * sizeof(float *));

    //Comprueba que se pueda reservar la memoria
    if ((jardin == NULL) || (tiempoUltimoSolGirasol == NULL) || (tiempoUltimoDisparo == NULL) || (vidaPlanta == NULL)) {
        printf("Error al reservar memoria\n");
        free(jardin);
        free(tiempoUltimoSolGirasol);
        free(tiempoUltimoDisparo);
        free(vidaPlanta);
        return 1;
    }

    //Dejamos espacio para cada 'matriz (en columnas ahora)
    for (int i = 0; i < filas; i++) {
        jardin[i] = malloc(columnas * sizeof(int));
        tiempoUltimoSolGirasol[i] = malloc(columnas * sizeof(double));
        tiempoUltimoDisparo[i] = malloc(columnas * sizeof(double));
        vidaPlanta[i] = malloc(columnas * sizeof(float));

        //Comprueba que se pueda guardar espacio, y si no se puede libera lo ya almacenado
        if ((jardin[i] == NULL) || (tiempoUltimoSolGirasol[i] == NULL) || (tiempoUltimoDisparo[i] == NULL) || (vidaPlanta[i] == NULL)) {
            free(jardin[i]);
            free(tiempoUltimoSolGirasol[i]);
            free(tiempoUltimoDisparo[i]);
            free(vidaPlanta[i]);
            printf("Error al reservar memoria.\n");
            for (int j = 0; j < i; j++) { 
                free(jardin[j]);
                free(tiempoUltimoSolGirasol[j]);
                free(tiempoUltimoDisparo[j]);
                free(vidaPlanta[j]);
            }
            free(jardin);
            free(tiempoUltimoSolGirasol);
            free(tiempoUltimoDisparo);
            free(vidaPlanta);

            return 1;
        }
    }



        //Inicializa las variables en sus valores normales (desactivados)
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            jardin[i][j] = VACIO;
            tiempoUltimoSolGirasol[i][j] = 0;
            tiempoUltimoDisparo[i][j] = 0;
            vidaPlanta[i][j] = 0;
        }
    }





    InitWindow(pxAncho, pxAlto, "Plantas vs Zombies (Probanding)"); //Se crea la ventana
    SetTargetFPS(67); //FPS de la ventana

    // Como las dimensiones son configurables, calculamos el tamaño de cada casilla
    float anchoCasilla = (float)anchoJardin/columnas;
    float altoCasilla = (float)altoJardin/filas;

    // Tamaño visual del zombie
    int anchoZombie = 40;
    int altoZombie = 60;

    int filaSeleccionada = -1;
    int columnaSeleccionada = -1;
    int plantaSeleccionada = VACIO;

    //Bucle de la pestaña abierta, esto es para que se ejecute el juego en si
    while (!WindowShouldClose()) { //Mientras no (!) se tenga que cerrar la ventana)

        //Primero declaramos las variables que se usan independiente del estado del juego
        Vector2 posMouse = GetMousePosition(); //devuelve la posicion x e y (con posMouse.x y posMouse.y se accede a c/u)
        double tiempoActual = GetTime(); //Variable que se usa todo el rato


        if (estadoJuego == JUGANDO) {
        /*
        Este primer bloque es de la logica de los clicks, detecta donde esta el mouse y donde se hace el click
        para despues decidir si corresponde a una casilla del jardin, a una de las plantas o a un lugar sin nada    
        */
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { //Verificamos que el click esta dentro del jardin
                int solRecogido = 0;
                for (int i = 0; i < SOLES_MAXIMOS; i++) { //Verificamos si hay un sol y hace click en el
                    if (CheckCollisionPointCircle(posMouse,(Vector2){.x=solesX[i],.y=solesY[i]},solesRadio) && (solesPantalla[i] == 1)) {
                        soles += 25;
                        solesPantalla[i] = 0;
                        solRecogido = 1;
                        solesTipo[i] = 0;
                        solesTiempo[i] = 0;
                        break;
                    }
                }
                if (solRecogido == 0) {
                    if ((posMouse.x>=pxDistanciaX) && (posMouse.x<(pxDistanciaX+anchoJardin)) && (posMouse.y>=pxDistanciaY) && (posMouse.y<(pxDistanciaY+altoJardin))) {
                        int casillax = ((posMouse.x - pxDistanciaX) / anchoCasilla);
                        columnaSeleccionada = casillax;
                        int casillay = ((posMouse.y - pxDistanciaY) / altoCasilla);
                        filaSeleccionada = casillay;
                        printf("Fila: %d | Columna: %d\n", casillay, casillax);

                        if ((plantaSeleccionada != VACIO) && (jardin[filaSeleccionada][columnaSeleccionada] == VACIO)){ //Se le asignan valores a las plantas
                            int costo = 0;
                            if (plantaSeleccionada == GIRASOL) {
                                costo = costoGirasol;
                            } else if (plantaSeleccionada == LANZAGUISANTES) {
                                costo = costoLanzaguisantes;
                            } else if (plantaSeleccionada == NUEZ) {
                                costo = costoNuez;
                            }

                            if (soles >= costo) {
                                jardin[filaSeleccionada][columnaSeleccionada] = plantaSeleccionada;

                                if (plantaSeleccionada == GIRASOL) {
                                    tiempoUltimoSolGirasol[filaSeleccionada][columnaSeleccionada] = GetTime();
                                    vidaPlanta[filaSeleccionada][columnaSeleccionada] = VIDA_GIRASOL;
                                } else if (plantaSeleccionada == LANZAGUISANTES) {
                                    tiempoUltimoDisparo[filaSeleccionada][columnaSeleccionada] = GetTime();
                                    vidaPlanta[filaSeleccionada][columnaSeleccionada] = VIDA_LANZAGUISANTES;
                                } else if (plantaSeleccionada == NUEZ) {
                                    vidaPlanta[filaSeleccionada][columnaSeleccionada] = VIDA_NUEZ;
                                }
                                soles -= costo;
                                plantaSeleccionada = VACIO;           
                            }
                        }

                    } else if ((posMouse.x>=64) && (posMouse.x<=204) && (posMouse.y>=140) && (posMouse.y<=540)) {
                        for (int i = 0; i < 3; i++) {
                            if ((posMouse.y>=140+80*i) && (posMouse.y<=140+80*(i+1))) {
                                plantaSeleccionada = i+1;
                            }
                        }
                    } else {
                        filaSeleccionada = -1;
                        columnaSeleccionada = -1;
                    }
                }
            }






        /*
        Este bloque es para el spawn d soles
        */
            for (int i = 0; i < filas; i++) {
                for (int j = 0; j < columnas; j++) {
                    if ((jardin[i][j] == GIRASOL) && (tiempoActual - tiempoUltimoSolGirasol[i][j] >= cooldownSol)) {
                        for (int k = 0; k < SOLES_MAXIMOS; k++) {
                            if (solesPantalla[k] == 0) {
                                solesPantalla[k] = 1;
                                solesTipo[k] = SOL_GIRASOL;
                                solesX[k] = pxDistanciaX + anchoCasilla / 2 + j * anchoCasilla;
                                solesY[k] = pxDistanciaY + altoCasilla / 2 + i * altoCasilla;
                                solesTiempo[k] = tiempoActual;
                                tiempoUltimoSolGirasol[i][j] = tiempoActual;

                                break;
                            }
                        }
                    }
                }
            }

            if (tiempoActual - tiempoUltimoSolCielo >= cooldownSolCielo) {
                for (int i = 0; i < SOLES_MAXIMOS; i++) {
                    if (solesPantalla[i] == 0) {
                        solesPantalla[i] = 1;
                        solesTipo[i] = SOL_CIELO;
                        solesX[i] = GetRandomValue(224,1376); //Valores pueden cambiar segun coso
                        solesY[i] = pxDistanciaY - solesRadio;
                        solesTiempo[i] = tiempoActual;

                        tiempoUltimoSolCielo = tiempoActual;
                        break;
                    }
                }
            }
            
            for (int i = 0; i < SOLES_MAXIMOS; i++) {
                if ((solesPantalla[i] == 1) && (solesTipo[i] == SOL_CIELO)) {
                    solesY[i] += velocidadSol *GetFrameTime();
                }
            }

            for (int i = 0; i < SOLES_MAXIMOS; i++) {
                if (solesPantalla[i] == 1) {
                    if (tiempoActual - solesTiempo[i] > duracionSol) {
                        solesPantalla[i] = 0;
                        solesTipo[i] = 0;
                        solesTiempo[i] = 0;
                    }
                }
            }





        /*
        Este bloque de codigo es para todas las mecanicas de los zombies
        */
            if ((tiempoActual - tiempoUltimoZombie >= cooldownZombie) && (rondaActiva == 1) && (zombiesGenerados < zombiesPorRonda)) {
                for (int i = 0; i < ZOMBIES_MAXIMOS; i++) {
                    if (zombies[i].vivo == MUERTO) {
                        int filaZombie = GetRandomValue(0, filas-1); //Para que spawnee en una de las filas aleatoriamente
                        zombies[i].x = pxDistanciaX + anchoJardin - anchoZombie;
                        zombies[i].y = pxDistanciaY + filaZombie * altoCasilla + ((altoCasilla - altoZombie) / 2);
                        zombies[i].tiempoUltimoAtaque = tiempoActual;

                        zombies[i].columnaAtaque = -1;
                        int tipoZombie = GetRandomValue (1,100);
                        if (tipoZombie <= probabilidadNormal) {
                            zombies[i].tipo = NORMAL;
                            zombies[i].velocidad = 20;
                            zombies[i].vida = 100;
                            zombies[i].estado = CAMINANDO;
                            zombies[i].daño = 10;
                            zombies[i].cooldownAtaque = 1.5;
                            
                        } else {
                            zombies[i].tipo = FLAG;
                            zombies[i].velocidad = 60;
                            zombies[i].vida = 150;
                            zombies[i].estado = CAMINANDO;
                            zombies[i].daño = 10;
                            zombies[i].cooldownAtaque = 1.5;
                        }
                        zombies[i].vivo = VIVO;
                        zombiesGenerados++;
                        break;
                    }
                }
                tiempoUltimoZombie = tiempoActual;
            }


        /*
        Deteccion de plantas en el camino del zombie
        */
            zombiesVivos = 0;
            for (int i = 0; i < ZOMBIES_MAXIMOS; i++) {
                if (zombies[i].vivo == VIVO) {
                    zombiesVivos++;
                    if (zombies[i].estado == CAMINANDO) {
                        zombies[i].x -= zombies[i].velocidad * GetFrameTime(); //Movimiento de los zombies
                        int filaZombie = (zombies[i].y - pxDistanciaY) / altoCasilla;
                        int columnaZombie = (zombies[i].x - pxDistanciaX) / anchoCasilla;
                        //Comprobamos si llego a la casa
                        if (zombies[i].x <= pxDistanciaX - (anchoZombie / 2)) {
                            zombieCasa++;
                            zombies[i].vivo = MUERTO;
                        } else if ((filaZombie >= 0 && filaZombie < filas) && (columnaZombie >= 0 && columnaZombie < columnas)) {
                            if (vidaPlanta[filaZombie][columnaZombie] > 0) {
                                zombies[i].estado = ATACANDO;
                                zombies[i].columnaAtaque = columnaZombie;
                                zombies[i].filaAtaque = filaZombie;
                            }
                        }
                    } else if (zombies[i].estado == ATACANDO) { 
                        if (vidaPlanta[zombies[i].filaAtaque][zombies[i].columnaAtaque] <= 0) {
                            jardin[zombies[i].filaAtaque][zombies[i].columnaAtaque] = VACIO;
                            vidaPlanta[zombies[i].filaAtaque][zombies[i].columnaAtaque] = 0;
                            zombies[i].estado = CAMINANDO;
                            zombies[i].columnaAtaque = -1;
                            zombies[i].filaAtaque = -1;
                        } else if (tiempoActual - zombies[i].tiempoUltimoAtaque >= zombies[i].cooldownAtaque) {
                            zombies[i].tiempoUltimoAtaque = tiempoActual;
                            vidaPlanta[zombies[i].filaAtaque][zombies[i].columnaAtaque] -= zombies[i].daño;
                        }
                    }
                }
            }






        /*
        Bloque para logica de proyectiles (plantas)
        */
            for (int i = 0; i < filas; i++) {
                for (int j = 0; j < columnas; j++) {
                    if (jardin[i][j] == LANZAGUISANTES) {
                        for (int k = 0; k < ZOMBIES_MAXIMOS; k++) {
                            if (zombies[k].vivo == VIVO) {
                                float pxSuperior = pxDistanciaY + i * altoCasilla;
                                float pxInferior = pxSuperior + altoCasilla;
                                if ((zombies[k].y >= pxSuperior) && (zombies[k].y < pxInferior) && (tiempoActual - tiempoUltimoDisparo[i][j] >= cooldownProyectil)) {
                                    for (int l = 0; l < PROYECTILES_MAXIMOS; l++) {
                                        if ((proyectiles[l].vivo == MUERTO) && (zombies[k].x > pxDistanciaX + j * anchoCasilla)) {
                                            proyectiles[l].vivo = VIVO;
                                            proyectiles[l].daño = 10;
                                            proyectiles[l].velocidad = 300;
                                            proyectiles[l].x = pxDistanciaX + j * anchoCasilla + anchoCasilla / 2;
                                            proyectiles[l].y = pxDistanciaY + i * altoCasilla + altoCasilla / 2;
                                            tiempoUltimoDisparo[i][j] = tiempoActual;
                                            break;
                                        }
                                    }
                                    break; //Sale del bucle de zombies
                                } 
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < PROYECTILES_MAXIMOS; i++) {
                if (proyectiles[i].vivo == VIVO) {
                    proyectiles[i].x += proyectiles[i].velocidad * GetFrameTime();
                }
            }
            
            for (int i = 0; i < PROYECTILES_MAXIMOS; i++) {
                if (proyectiles[i].vivo == VIVO) {
                    if (proyectiles[i].x >= pxDistanciaX + anchoJardin) {
                        proyectiles[i].vivo = MUERTO;
                    } else {
                        for (int j = 0; j < ZOMBIES_MAXIMOS; j++) {
                            if (zombies[j].vivo == VIVO) {
                                if (CheckCollisionCircleRec((Vector2){proyectiles[i].x,proyectiles[i].y}, proyectilesRadio, (Rectangle){zombies[j].x, zombies[j].y, anchoZombie, altoZombie})) {
                                    zombies[j].vida -= proyectiles[i].daño;
                                    if (zombies[j].vida <= 0) {
                                        zombies[j].vivo = MUERTO;
                                        proyectiles[i].vivo = MUERTO;
                                        break;
                                    }
                                    proyectiles[i].vivo = MUERTO;
                                }
                            }
                        }
                    }
                }
            }







        /*
        Este bloque es para calcular la ronda actual
        */
            if (zombieCasa >= vidaCasa) {
                estadoJuego = LOSE;
            } else if ((rondaActiva == 1) && (zombiesVivos == 0) && (zombiesGenerados >= zombiesPorRonda)) {
                if (rondaActual == cantidadRondas) {
                    estadoJuego = WIN;
                } else {
                    rondaActiva = 0;
                    rondaActual++;
                    tiempoInicioGracia = tiempoActual;
                }
            } else if (rondaActiva == 0) {
                if (tiempoActual - tiempoInicioGracia >= duracionGracia) {
                    rondaActiva = 1;
                    zombiesGenerados = 0;
                }
            }
        }







        //Para dibujar todo |el fondo|
        BeginDrawing();
        ClearBackground(DARKBROWN);

        DrawFPS(10,10);
        DrawText("PVZ (FRUNA)", 770, 35, 50, WHITE);


        if (estadoJuego == JUGANDO) {
            //Contador de soles
            DrawRectangle(64, 81, 140, 50, DARKGRAY);
            DrawText(TextFormat("Soles: %d",soles), 75, 95, 25, YELLOW);
            // Dibujamos el menu de las plantas
            for (int i = 0; i < 3; i++) { 
                DrawRectangle(64, 80*i+140, 140, 75, DARKBLUE);
            }


            // Dibujamos el jardin
            for (int i = 0; i < filas; i++) {
                for (int j = 0; j < columnas; j++) {
                    int x = pxDistanciaX + j * anchoCasilla; //Calculamos la posicion en px en el eje X
                    int y = pxDistanciaY + i * altoCasilla; //Calculamos la posicion en px en el eje Y

                    //Asignamos un color a cada casilla de manera intercalada
                    if ((filaSeleccionada == i) && (columnaSeleccionada == j)) {
                        DrawRectangle(x, y, anchoCasilla, altoCasilla, (Color){15,79,32,255});
                    } else if (((i+j) % 2) == 0) { 
                        DrawRectangle(x, y, anchoCasilla, altoCasilla, GREEN);
                    } else {
                        DrawRectangle(x, y, anchoCasilla, altoCasilla, DARKGREEN);
                    }

                    if (jardin[i][j] == GIRASOL) {
                        DrawRectangle(x+5, y+5, anchoCasilla-10, altoCasilla-10, PINK);
                    } else if (jardin[i][j] == LANZAGUISANTES) {
                        DrawRectangle(x+5, y+5, anchoCasilla-10, altoCasilla-10, ORANGE);
                    } else if (jardin[i][j] == NUEZ) {
                        DrawRectangle(x+5, y+5, anchoCasilla-10, altoCasilla-10, BLACK);
                    }

                    DrawRectangleLines(x, y,anchoCasilla, altoCasilla, BLACK);
                }
            }

            
            // Dibujo spawn de soles
            for (int i = 0; i < SOLES_MAXIMOS; i++) {
                if (solesPantalla[i] == 1) {
                DrawCircle(solesX[i], solesY[i], solesRadio, YELLOW);
                }
            }

            // Dibujamos los zombies
            for (int i = 0; i < ZOMBIES_MAXIMOS; i++) {
                if (zombies[i].vivo == VIVO) {
                    if (zombies[i].tipo == NORMAL) {
                        DrawRectangle(zombies[i].x, zombies[i].y, anchoZombie, altoZombie, VIOLET);
                    } else if (zombies[i].tipo == FLAG) {
                        DrawRectangle(zombies[i].x, zombies[i].y, anchoZombie, altoZombie, PURPLE);
                    }
                }
            }

            // Dibujamos los proyectiles
            for (int i = 0; i < PROYECTILES_MAXIMOS; i++) {
                if (proyectiles[i].vivo == VIVO) {
                    DrawCircle(proyectiles[i].x, proyectiles[i].y, proyectilesRadio, GREEN);
                    DrawCircleLines(proyectiles[i].x, proyectiles[i].y, proyectilesRadio+1, BLACK);
                }
            }


        } else if (estadoJuego == MENU) {
            int pxDistanciaBotonX = pxAncho / 6;
            int pxAnchoBoton = pxAncho * 2 / 3;
            int pxAltoBoton = pxDistanciaY * 2; 
            for (int i = 0; i < 3; i++) {
                DrawRectangle(pxDistanciaBotonX, pxDistanciaY + (i * 210), pxAnchoBoton, pxAltoBoton, ORANGE);
            }
            DrawText("JUGAR",pxAnchoBoton / 2 , pxDistanciaY + 40, 80, BLACK); //FALTA CENTRAR Y CAMBIAR FUENTE
            DrawText("CONFIGURACIÓN",pxAnchoBoton / 2 , pxDistanciaY + 250, 80, BLACK); //FALTA CENTRAR Y CAMBIAR FUENTE
            DrawText("SALIR",pxAnchoBoton / 2 , pxDistanciaY + 460, 80, BLACK); //FALTA CENTRAR Y CAMBIAR FUENTE

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if ((posMouse.x >= pxDistanciaBotonX) && (posMouse.x <= pxDistanciaBotonX + pxAnchoBoton) && (posMouse.y >= pxDistanciaY) && (posMouse.y <= pxDistanciaY + pxAltoBoton)) {
                    estadoJuego = JUGANDO;
                    tiempoInicioGracia = GetTime(); //Para que empiece el primer periodo de preparacion

                } else  if ((posMouse.x >= pxDistanciaBotonX) && (posMouse.x <= pxDistanciaBotonX + pxAnchoBoton) && (posMouse.y >= pxDistanciaY + 210) && (posMouse.y <= pxDistanciaY + 210 + pxAltoBoton)) {
                    //HACER AJUSTES || MUSICA Y ¿

                } else if ((posMouse.x >= pxDistanciaBotonX) && (posMouse.x <= pxDistanciaBotonX + pxAnchoBoton) && (posMouse.y >= pxDistanciaY + 420) && (posMouse.y <= pxDistanciaY + 420 + pxAltoBoton)) {
                    break;
                }
            }


        } else if (estadoJuego == WIN) {
            DrawText("GANASTE",200, pxDistanciaY + 40, 80, BLACK); //FALTA CENTRAR Y CAMBIAR FUENTE


        } else if (estadoJuego == LOSE) {
            DrawText("PERDISTE",200, pxDistanciaY + 40, 80, BLACK); //FALTA CENTRAR Y CAMBIAR FUENTE
        }


        EndDrawing();
    }
    CloseWindow();



    // Liberamos la memoria
    for (int i = 0; i < filas; i++) {
        free(jardin[i]);
        free(tiempoUltimoSolGirasol[i]);
        free(tiempoUltimoDisparo[i]);
        free(vidaPlanta[i]);
    }
    free(jardin);
    free(tiempoUltimoSolGirasol);
    free(tiempoUltimoDisparo);
    free(vidaPlanta);
    
    return 0;
}
