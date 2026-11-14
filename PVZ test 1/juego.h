#ifndef JUEGO_H
#define JUEGO_H
#include "raylib.h"

//Estados generales del juego
#define MENU 0 
#define JUGANDO 1
#define WIN 2
#define LOSE 3

//Definiciones para los tipos de plantas de la matriz del jardin
#define VACIO 0
#define GIRASOL 1
#define LANZAGUISANTES 2
#define NUEZ 3
#define METRALLA 4
#define CONGELADORA 5
#define CEREZA 6
#define PALA 7
#define OBSTACULO 9
#define MARGEN_OBSTACULO 4 //Cuantas columnas no pueden tener obstaculos de izquierda a derecha

// Definir vida inicial de las plantas
#define VIDA_GIRASOL 40
#define VIDA_LANZAGUISANTES 40
#define VIDA_NUEZ 250
#define VIDA_METRALLA 40
#define VIDA_CONGELADORA 40
#define VIDA_CEREZA 100

//Para diferenciar los tipos de soles
#define SOL_GIRASOL 1
#define SOL_CIELO 2
#define SOLES_MAXIMOS 50 // Cantidad maxima de soles al mismo tiempo

//Caracteristicas de la cereza
#define DANO_CEREZA 150
#define TIEMPO_CEREZA 1.0

// Para diferenciar los tipos de zombies
#define NORMAL 1
#define FLAG 2
#define ZOMBIES_MAXIMOS 50 //Cantidad maxima de zombies al mismo tiempo

// Estado del comportamiento de los zombies
#define CAMINANDO 1
#define ATACANDO 2

#define PROYECTILES_MAXIMOS 100 // Cantidad maxima de proyectiles al mismo tiempo

//Estados para los zombies y proyectiles
#define MUERTO 0
#define VIVO 1

// Para realentizar zombies
#define NO_CONGELADO 0
#define CONGELADO 1

//Aqui estan todas las funciones de raylib: https://www.raylib.com/cheatsheet/cheatsheet.html

int cargarConfig(const char *nombreArchivo);
int cargarJardin(const char *nombreArchivo);

/*
Aclarar que lo primero de logica que se hizo fue todo lo de los soles, al ser lo primero
no se hizo con struct, despues entendimos como funciona el struct y lo implementamos para
los zombies y los proyectiles. No volvimos a hacer los soles con struct para tener tiempo
suficiente para implementar las mejoras que ya teniamos pensadas
*/

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
    int dano;
    float cooldownAtaque;
    double tiempoUltimoAtaque;
    int congelado;
    double tiempoCongelado;
} Zombie; 

//Struct con las stats de los proyectiles
typedef struct {
    float x;
    float y;
    int dano;
    float velocidad;
    int tipo;
    int vivo;
} Proyectil;



// Variables globales del juego

// Estado general
extern int estadoJuego;
extern int filas;
extern int columnas;
extern int cantidadObstaculos;

// Variables de las rondas y de la casa
extern int vidaCasa;
extern int zombieCasa;
extern int cantidadRondas;
extern double duracionGracia;
extern double tiempoInicioGracia;
extern int rondaActual;
extern int rondaActiva;

// Dimensiones de la ventana y del jardin
extern int pxAncho;
extern int pxAlto;
extern int pxDistanciaX;
extern int pxDistanciaY;
extern int anchoJardin;
extern int altoJardin;
extern float anchoCasilla;
extern float altoCasilla;

// Variables de los soles
extern int soles;
extern float solesRadio;
extern float solesX[SOLES_MAXIMOS];
extern float solesY[SOLES_MAXIMOS];
extern int solesTipo[SOLES_MAXIMOS];
extern int solesPantalla[SOLES_MAXIMOS];
extern double solesTiempo[SOLES_MAXIMOS];
extern double cooldownSol;
extern double duracionSol;
extern float velocidadSol;
extern double tiempoUltimoSolCielo;
extern double cooldownSolCielo;

// Variables de los zombies
extern double tiempoUltimoZombie;
extern float cooldownZombie;
extern int zombiesGenerados;
extern int zombiesPorRonda;
extern int zombiesVivos;
extern float probabilidadNormal;
extern int anchoZombie;
extern int altoZombie;
extern Zombie zombies[ZOMBIES_MAXIMOS];

// Variables de los proyectiles
extern double cooldownLanzaguisantes;
extern double cooldownMetralla;
extern float proyectilesRadio;
extern double duracionCongelacion;
extern double multRalentizacion;
extern Proyectil proyectiles[PROYECTILES_MAXIMOS];

// Variables de seleccion de plantas y costos
extern int costoGirasol;
extern int costoLanzaguisantes;
extern int costoNuez;
extern int costoMetralla;
extern int costoCongeladora;
extern int costoCereza;
extern int filaSeleccionada;
extern int columnaSeleccionada;
extern int plantaSeleccionada;

//Variables de la cereza
extern int cerezaActiva;
extern int filaCereza;
extern int columnaCereza;
extern double tiempoCereza;

// Matrices dinamicas del jardin
extern int **jardin;
extern double **tiempoUltimoSolGirasol;
extern double **tiempoUltimoDisparo;
extern int **vidaPlanta;





// Funciones del juego

// Funciones para crear, inicializar y liberar matrices (memoria dinamica)
int **crearMatrizInt(int filas, int columnas);
double **crearMatrizDouble(int filas, int columnas);
int inicializarMatrices(void);
void liberarMatrices(void);

int generarObstaculos(void);

//Funciones para actualizar las mecanicas del juegp
void actualizarSoles(double tiempoActual);
void generarZombies(double tiempoActual);
void actualizarZombies(double tiempoActual);
void actualizarProyectiles(double tiempoActual);
void actualizarRondas(double tiempoActual);
void actualizarCereza(double tiempoActual);

//Funcion para procesar las interacciones del jugador
void procesarClicks(Vector2 posMouse);
#endif
