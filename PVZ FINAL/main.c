#include "juego.h"
#include <stdio.h>
#include <time.h>
#include "graficos.h"


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
int soles = 200;
float solesRadio = 21.0;
float solesX[SOLES_MAXIMOS];
float solesY[SOLES_MAXIMOS];
int solesTipo[SOLES_MAXIMOS];
int solesPantalla[SOLES_MAXIMOS];
double solesTiempo[SOLES_MAXIMOS];
double cooldownSol = 8.0;
double duracionSol = 5.0;
float velocidadSol = 40.0;
double tiempoUltimoSolCielo = 0;
double cooldownSolCielo = 13.0;

// Variables de los zombies
double tiempoUltimoZombie = 0;
float cooldownZombie = 8.0;
int zombiesGenerados = 0;
int zombiesPorRonda = 10;
int zombiesVivos = 0;
float probabilidadNormal = 80;
float probabilidadBalde = 5;
Zombie zombies[ZOMBIES_MAXIMOS];

// Variables de los proyectiles
double cooldownLanzaguisantes = 2.0;
double cooldownMetralla = 0.9;
double duracionCongelacion = 2.3;
double multRalentizacion = 0.5;
float proyectilesRadio = 13.0;
Proyectil proyectiles[PROYECTILES_MAXIMOS];

// Variables de seleccion de plantas
int costoGirasol = 50;
int costoLanzaguisantes = 100;
int costoNuez = 50;
int costoMetralla = 200;
int costoCongeladora = 200;
int costoCereza = 150;
int filaSeleccionada = -1;
int columnaSeleccionada = -1;
int plantaSeleccionada = VACIO;

//Variables de la cereza
int cerezaActiva = 0;
int filaCereza = -1;
int columnaCereza = -1;
double tiempoCereza = 0;

//Punteros a las matrices dinamicas
int **jardin;
double **tiempoUltimoSolGirasol;
double **tiempoUltimoDisparo;
int **vidaPlanta;


int main() {
    SetRandomSeed((unsigned int)time(NULL)); //Inicializar la semilla de aleatoriedad de raylib

    //Cargamos la configuracion
    if (cargarConfig("Config.txt") == 0) {
        printf("Error al cargar Config.txt\n");
        return 1;
    }

    //Cargamos el jardin
    if (cargarJardin("Jardin.txt") == 0) {
        printf("Error al cargar Jardin.txt\n");
        return 1;
    }

    //Generamos los obstaculos que haya pedido el usuario
    if (generarObstaculos() == 0) {
        printf("Error al generar los obstaculos\n");
        liberarMatrices();
        return 1;
    }

    anchoCasilla = (float)anchoJardin / columnas;
    altoCasilla = (float)altoJardin / filas;

    
    InitWindow(pxAncho, pxAlto, "Plantas vs Zombies"); //Se crea la ventana donde se ejecuta el juego
    SetTargetFPS(60); //Establecemos los FPS del juego
    cargarTexturas(); //Para cargar las imagenes

    //Bucle mientras la pestaña este abierta, esto es para que se ejecute el juego en si
    while (!WindowShouldClose()) { 

        //Primero declaramos las variables que se usan independiente del estado del juego
        Vector2 posMouse = GetMousePosition(); //devuelve la posicion x e y (con posMouse.x y posMouse.y se accede a c/u)
        double tiempoActual = GetTime();

        /*
        El juego esta separado en 4 secciones:
        0- MENU: Aqui se inicia la partida, y ademas se puede cerrar el juego
        1- JUGANDO: Todo el juego en si, es la logica de la partida
        2- WIN: Pantalla que aparece cuando el usuario gana
        3- LOSE: Pantalla que aparece cuando el usuario pierde
        */

        if (estadoJuego == JUGANDO) {
            //Generamos los soles, actualizamos su movimiento y verificamos su despawneo
            generarSolesGirasoles(tiempoActual);
            generarSolesCielo(tiempoActual);
            moverSolesCielo();
            desaparecerSoles(tiempoActual);

            //Generacion de los zombies
            generarZombies(tiempoActual);

            //Actualizamos los estados, movimientos y ataques de los zombies
            congelacionZombies(tiempoActual);
            moverZombies();
            detectarPlantasZombies();
            actualizarAtaquesZombies(tiempoActual);

            //Se generan los proyectiles, se mueven y detectan colisiones
            generarProyectiles(tiempoActual);
            moverProyectiles();
            detectarColisionesProyectiles(tiempoActual);

            //Para detectar y actualizar cerezas
            actualizarCereza(tiempoActual);

            //Actualizar la cantidad de zombies vivos y comprobamos el estado de la ronda
            contarZombiesVivos();
            actualizarRondas(tiempoActual);

            //Para detectar y procesar clicks
            procesarClicks(posMouse);

        }



        if ((estadoJuego == MENU) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(posMouse, botonMenu(0))) {
                estadoJuego = JUGANDO;
                tiempoInicioGracia = GetTime(); //Para que empiece el primer periodo de preparacion

            } else if (CheckCollisionPointRec(posMouse, botonMenu(1))) {
                break; //Sale del while y se cierra el juego
            }
        }



        //Dibujo de los elementos del juego (el orden determina que elementos quedan encima)
        BeginDrawing();
        ClearBackground(DARKBROWN);

        if (estadoJuego == JUGANDO) {
            dibujarHUD();
            dibujarContadorSoles();
            dibujarMenuPlantas();
            dibujarJardin();
            dibujarCereza();
            dibujarVistaPrevia();
            dibujarZombies();
            dibujarProyectiles();
            dibujarSoles();
            dibujarAvisoOleada();
            dibujarCursorPala();

        } else if (estadoJuego == MENU) {
            dibujarMenu();

        } else if (estadoJuego == WIN) {
            dibujarPantallaFinal(1);

        } else if (estadoJuego == LOSE) {
            dibujarPantallaFinal(0);
        }

        DrawFPS(10, 10);
        EndDrawing();
    }
    liberarTexturas();
    CloseWindow();



    // Liberamos la memoria
    liberarMatrices();    
    return 0;
}