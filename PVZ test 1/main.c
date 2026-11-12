#include "juego.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// Variables globales del juego

// Estado general
int estadoJuego = MENU;
int filas = 5;
int columnas = 9;
int cantidadObstaculos = 0;

// Variables de las rondas y de la casa
int vidaCasa = 3; 
int zombieCasa = 0; 
int cantidadRondas = 5;
double duracionGracia = 8.0;
double tiempoInicioGracia = 0;
int rondaActual = 1;
int rondaActiva = 0;

// Dimensiones de la ventana y del jardin
int pxAncho = 1440;
int pxAlto = 810;
int pxDistanciaX = 224;
int pxDistanciaY = 81;
int anchoJardin = 1152;
int altoJardin = 648;
float anchoCasilla = 0;
float altoCasilla = 0;

// Variables de los soles
int soles = 300;
float solesRadio = 17.0;
float solesX[SOLES_MAXIMOS];
float solesY[SOLES_MAXIMOS];
int solesTipo[SOLES_MAXIMOS];
int solesPantalla[SOLES_MAXIMOS];
double solesTiempo[SOLES_MAXIMOS];
double cooldownSol = 7.0;
double duracionSol = 4.5;
float velocidadSol = 40.0;
double tiempoUltimoSolCielo = 0;
double cooldownSolCielo = 10.0;

// Variables de los zombies
double tiempoUltimoZombie = 0;
float cooldownZombie = 8.0;
int zombiesGenerados = 0;
int zombiesPorRonda = 10;
int zombiesVivos = 0;
float probabilidadNormal = 80;
int anchoZombie = 40;
int altoZombie = 60;
Zombie zombies[ZOMBIES_MAXIMOS];

// Variables de los proyectiles
double cooldownLanzaguisantes = 2.0;
double cooldownMetralla = 0.9;
float proyectilesRadio = 13.0;
Proyectil proyectiles[PROYECTILES_MAXIMOS];

// Variables de seleccion de plantas
int costoGirasol = 50;
int costoLanzaguisantes = 100;
int costoNuez = 50;
int costoMetralla = 200;
int filaSeleccionada = -1;
int columnaSeleccionada = -1;
int plantaSeleccionada = VACIO;

//Punteros a las matrices dinamicas
int **jardin;
double **tiempoUltimoSolGirasol;
double **tiempoUltimoDisparo;
int **vidaPlanta;


int main() {
    SetRandomSeed((unsigned int)time(NULL));; //Inicializar la semilla de aleatoriedad para rand()

    if (cargarConfig("Config.txt") == 0) {
        printf("Error al cargar Config.txt\n");
        return 1;
    }

    if (cargarJardin("Jardin.txt") == 0) {
        printf("Error al cargar Jardin.txt\n");
        return 1;
    }

    if (generarObstaculos() == 0) {
        printf("Error al generar los obstaculos\n");
        liberarMatrices();
        return 1;
    }

    anchoCasilla = (float)anchoJardin / columnas;
    altoCasilla = (float)altoJardin / filas;


    /*
    En esta parte usamos la memoria dinamica para establecer el jardin en si, ademas la aprovechamos 
    para calcular el tiempo en que se genero el ultimo sol de cada girasol, el tiempo del ultimo 
    disparo de cada lanzaguistantes y la vida de cada planta
    
    Hicimos estas 4 cosas con memoria dinamica porque todas tienen mecanicas en funcion de filas y 
    columnas, o sea que son estables, no como las cosas que dependen de la distancia en pixeles
    */
    InitWindow(pxAncho, pxAlto, "Plantas vs Zombies"); //Se crea la ventana donde se ejecuta el juego
    SetTargetFPS(60); //Establecemos los FPS del juego

    //Bucle mientras la pestaña este abierta, esto es para que se ejecute el juego en si
    while (!WindowShouldClose()) { //Mientras no (!) se tenga que cerrar la ventana

        //Primero declaramos las variables que se usan independiente del estado del juego
        Vector2 posMouse = GetMousePosition(); //devuelve la posicion x e y (con posMouse.x y posMouse.y se accede a c/u)
        double tiempoActual = GetTime();

        /*
        El juego esta separado en 4 grandes secciones:
        0- MENU: Aqui se inicia la partida, y ademas se puede cerrar el juego
        1- JUGANDO: Todo el juego en si, es la logica de la partida
        2- WIN: Pantalla que aparece cuando el usuario gana
        3- LOSE: Pantalla que aparece cuando el usuario pierde
        */

        if (estadoJuego == JUGANDO) {
        /*
        El bloque de jugando es el mas extenso ya que incluye casi toda la logica, asi que esta subdividido en
        diferentes bloques para darle mas orden y que se entienda mejor

        Este primer bloque es de la logica de los clicks, detecta donde se hace el click para despues decidir si 
        corresponde a una casilla del jardin, a un sol, a una planta del menu o a un lugar sin nada seleccionable  
        */
            

            //Para la generacion, movimiento y desaparicion de soles
            actualizarSoles(tiempoActual);

            //Para la generacion de los zombies
            generarZombies(tiempoActual);

            //Para actualizar los zombies
            actualizarZombies(tiempoActual);

            //Para actualizar los proyectiles
            actualizarProyectiles(tiempoActual);

            //Para actualizar las rondas y comprobar si el jugador gano o perdio
            actualizarRondas(tiempoActual);

            //Para detectar y procesar clicks
            procesarClicks(posMouse);

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
            for (int i = 0; i < 7; i++) { 
                DrawRectangle(64, 140 + i * 85, 140, 75, DARKBLUE);
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
                    } else if (jardin[i][j] == METRALLA) {
                        DrawRectangle(x+5, y+5, anchoCasilla-10, altoCasilla-10, GOLD);
                    } else if (jardin[i][j] == OBSTACULO) {
                        DrawRectangle(x+5, y+5, anchoCasilla-10, altoCasilla-10, GRAY);
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
    liberarMatrices();    
    return 0;
}