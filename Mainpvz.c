#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

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

// Definir vida inicial de las plantas
#define VIDA_GIRASOL 40
#define VIDA_LANZAGUISANTES 40
#define VIDA_NUEZ 200

//Para diferenciar los tipos de soles
#define SOL_GIRASOL 1
#define SOL_CIELO 2
#define SOLES_MAXIMOS 50 // Cantidad maxima de soles al mismo tiempo

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

//Aqui estan todas las funciones de raylib: https://www.raylib.com/cheatsheet/cheatsheet.html

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




//Aqui inicia el programa, esta parte inicializa todas las variables y ejecuta el programa
int main(void) {
    int estadoJuego = MENU; //El juego se inicia en el menu

    // Ajustamos el tamaño del patio
    int filas = 5;
    int columnas = 9;

    /*
    Victoria: decidimos que el juego tenga un numero limitado de rondas, al completar la ultima ronda sin
    haber superado el limite de zombies en la casa, entonces el jugador gana

    Derrota: decidimos que lo mejor era que la casa pueda soportar un numero limitado de zombies (vidaCasa), 
    y que si los zombies que llegan a la casa (zombieCasa) igualan este numero, entonces el jugador pierde
    */
   
    // Variables de las rondas y de la casa
    int vidaCasa = 3; //Cuantos zombies pueden llegar a la casa
    int zombieCasa = 0; //Cuantos zombies han llegado a la casa
    int cantidadRondas = 5;
    double duracionGracia = 8.0; //Tiempo antes de que empiece una ronda
    double tiempoInicioGracia = GetTime(); 
    int rondaActual = 1; //Contador de que ronda se esta jugando
    int rondaActiva = 0; // 0 = no hay ronda activa, 1 = si hay ronda activa

    // Dimensiones de la ventana donde se abre el juego
    int pxAncho = 1440; //1440
    int pxAlto = 810; // 810

    // Dimensiones del jardin
    int pxDistanciaX = 224; //Distancia de izquierda a donde empieza el jardin
    int pxDistanciaY = 81; //Distancia de arriba a donde empieza el jardin
    int anchoJardin = 1152; //Ancho total del jardin
    int altoJardin = 648; //Alto total del jardin

    // Soles iniciales y costos
    int soles = 100;
    int costoGirasol = 50;
    int costoLanzaguisantes = 100;
    int costoNuez = 50;

    // Variables para los soles que spawnean
    float solesRadio = 17.0;
    float solesX[SOLES_MAXIMOS]; //Coords eje x
    float solesY[SOLES_MAXIMOS]; //Coords eje y
    int solesTipo[SOLES_MAXIMOS]; //Tipo de sol (cielo o girasol)
    int solesPantalla[SOLES_MAXIMOS]; //Valor booleano de si hay un sol en esa posicion
    double solesTiempo[SOLES_MAXIMOS]; //Tiempo en el que aparecio el sol
    for (int i = 0; i < SOLES_MAXIMOS; i++) { //Inicializamos todos los espacios de los arreglos
        solesX[i] = 0;
        solesY[i] = 0;
        solesTipo[i] = 0;
        solesPantalla[i] = 0;
        solesTiempo[i] = 0;
    }
    double cooldownSol = 7.0; //Cada cuanto producen los girasoles
    double duracionSol = 4.5; //Tiempo que dura antes de despawnear
    float velocidadSol = 40.0; //Velocidad a la que bajan del cielo
    double tiempoUltimoSolCielo = 0;
    double cooldownSolCielo = 10.0;  //Cada cuanto aparecen soles del cielo

    //Variables para los zombies
    double tiempoUltimoZombie = 0;
    float cooldownZombie = 8.0; //Cada cuanto se genera 
    int zombiesGenerados = 0; //Cuantos han spawneado esta ronda
    int zombiesPorRonda = 10;
    int zombiesVivos = 0; //Cuantos zombies hay vivos en el instante
    //Probabilidad de cada zombie
    float probabilidadNormal = 80;

    //Variables para los proyectiles
    float cooldownProyectil = 2.0;
    float proyectilesRadio = 13.0;

    Zombie zombies[ZOMBIES_MAXIMOS]; //Declaramos zombies como el arreglo que tiene todas las stats de los zombies
    for (int i = 0; i < ZOMBIES_MAXIMOS; i++) { //Inicializamos todos los zombies como muertos
        zombies[i].vivo = MUERTO;
    }

    Proyectil proyectiles[PROYECTILES_MAXIMOS]; //Declaramos proyectiles como el arreglo que tiene todas las stats de los proyectiles
    for (int i = 0; i < PROYECTILES_MAXIMOS; i++) { //Inicializamos todos los proyectiles como muertos
        proyectiles[i].vivo = MUERTO;
    }





    /*
    En esta parte usamos la memoria dinamica para establecer el jardin en si, ademas la aprovechamos 
    para calcular el tiempo en que se genero el ultimo sol de cada girasol, el tiempo del ultimo 
    disparo de cada lanzaguistantes y la vida de cada planta
    
    Hicimos estas 4 cosas con memoria dinamica porque todas tienen mecanicas en funcion de filas y 
    columnas, o sea que son estables, no como las cosas que dependen de la distancia en pixeles
    */
    //Reservamos memoria para los punteros de las filas de cada matriz
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

    //Reservamos memoria para las columnas de cada fila
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

            return 1; //Finalizamos el programa deibod al error
        }
    }

    //Inicializa todos los espacios de las matrices en sus valores normales (desactivados)
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            jardin[i][j] = VACIO;
            tiempoUltimoSolGirasol[i][j] = 0;
            tiempoUltimoDisparo[i][j] = 0;
            vidaPlanta[i][j] = 0;
        }
    }





    InitWindow(pxAncho, pxAlto, "Plantas vs Zombies"); //Se crea la ventana donde se ejecuta el juego
    SetTargetFPS(60); //Establecemos los FPS del juego

    // Como las dimensiones del jardin son configurables, calculamos el tamaño de cada casilla
    float anchoCasilla = (float)anchoJardin/columnas;
    float altoCasilla = (float)altoJardin/filas;

    // Tamaño que ocupan los zombies
    int anchoZombie = 40;
    int altoZombie = 60;

    //Inicializamos la posicion del jardin que tiene seleccionada el usuario
    int filaSeleccionada = -1;
    int columnaSeleccionada = -1;
    int plantaSeleccionada = VACIO;

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
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { //Detectamos si el usuario hizo click izquierdo

                //Primero verificamos si hay un sol en esa posicion
                int solRecogido = 0;
                for (int i = 0; i < SOLES_MAXIMOS; i++) {
                    if (CheckCollisionPointCircle(posMouse,(Vector2){.x=solesX[i],.y=solesY[i]},solesRadio) && (solesPantalla[i] == 1)) {
                        //Si hay un sol, entonces se suma a los soles totales y se reinician su valores en los arreglos
                        soles += 25; 
                        solesPantalla[i] = 0;
                        solRecogido = 1;
                        solesTipo[i] = 0;
                        solesTiempo[i] = 0;
                        break;
                    }
                }

                //Si no se hizo click sobre ningun sol, entonces verifica que el click este dentro del jardin
                if (solRecogido == 0) {
                    if ((posMouse.x>=pxDistanciaX) && (posMouse.x<(pxDistanciaX+anchoJardin)) && (posMouse.y>=pxDistanciaY) && (posMouse.y<(pxDistanciaY+altoJardin))) {
                        //Si el click esta dentro del jardin pasa su valor de pixeles a filas y columnas
                        int casillax = ((posMouse.x - pxDistanciaX) / anchoCasilla);
                        columnaSeleccionada = casillax;
                        int casillay = ((posMouse.y - pxDistanciaY) / altoCasilla);
                        filaSeleccionada = casillay;

                        //Verifica si el usuario tiene seleccionada una planta y si en esa posicion del jardin no hay nada plantado
                        if ((plantaSeleccionada != VACIO) && (jardin[filaSeleccionada][columnaSeleccionada] == VACIO)){
                            //Se asigna el costo correspondiente segun el tipo de planta seleccionada
                            int costo = 0;
                            if (plantaSeleccionada == GIRASOL) {
                                costo = costoGirasol;
                            } else if (plantaSeleccionada == LANZAGUISANTES) {
                                costo = costoLanzaguisantes;
                            } else if (plantaSeleccionada == NUEZ) {
                                costo = costoNuez;
                            }

                            //Si el usuario tiene suficientes soles, entonces la planta en ese lugar
                            if (soles >= costo) {
                                jardin[filaSeleccionada][columnaSeleccionada] = plantaSeleccionada;


                            /*
                            Si la planta es un girasol, ademas de plantarla inicia el contador para el spawneo de soles
                            Si la planta es un lanzaguisantes, ademas de plantarla inicia el cooldown entre disparos

                            Ademas, a todas las plantas se les asigna su vida y se les resta su costo
                            */
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
                                plantaSeleccionada = VACIO; //Se reinicia la planta seleccionada       
                            }
                        }

                    //Se verifica si el click fue en la zona de seleccionar plantas
                    } else if ((posMouse.x>=64) && (posMouse.x<=204) && (posMouse.y>=140) && (posMouse.y<=540)) {
                        for (int i = 0; i < 3; i++) { //Determinamos que planta selecciono el usuario
                            if ((posMouse.y>=140+80*i) && (posMouse.y<=140+80*(i+1))) {
                                plantaSeleccionada = i+1;
                            }
                        }

                    //Si el click no es en ninguno de los lugares interactuables, entonces reinicia la casilla seleccionada
                    } else {
                        filaSeleccionada = -1;
                        columnaSeleccionada = -1;
                    }
                }
            }






        /*
        Este bloque es para el spawn de soles, ya sean de girasoles o del cielo
        */
        //Primero recorremos todas las casillas del jardin buscando girasoles y si ya paso su cooldown de spawnear soles
            for (int i = 0; i < filas; i++) {
                for (int j = 0; j < columnas; j++) {
                    if ((jardin[i][j] == GIRASOL) && (tiempoActual - tiempoUltimoSolGirasol[i][j] >= cooldownSol)) {
                        for (int k = 0; k < SOLES_MAXIMOS; k++) {
                            if (solesPantalla[k] == 0) { //Si se cumplen las condiciones, genera un sol
                                solesPantalla[k] = 1;
                                solesTipo[k] = SOL_GIRASOL; //Declara que es un sol de girasol
                                solesX[k] = pxDistanciaX + anchoCasilla / 2 + j * anchoCasilla; //Calcula los pixeles del spawn
                                solesY[k] = pxDistanciaY + altoCasilla / 2 + i * altoCasilla;
                                solesTiempo[k] = tiempoActual;
                                tiempoUltimoSolGirasol[i][j] = tiempoActual;

                                break; //Al generar el sol, sale del bucle
                            }
                        }
                    }
                }
            }

            // Verificamos si paso el cooldown de generacion de sol del cielo
            if (tiempoActual - tiempoUltimoSolCielo >= cooldownSolCielo) {
                for (int i = 0; i < SOLES_MAXIMOS; i++) {
                    if (solesPantalla[i] == 0) {
                        solesPantalla[i] = 1;
                        solesTipo[i] = SOL_CIELO; //Declara que es un sol del cielo
                        solesX[i] = GetRandomValue(pxDistanciaX, pxDistanciaX + anchoJardin); //Genera una posicion horizontal aleatoria dentro del jardin
                        solesY[i] = pxDistanciaY - solesRadio;
                        solesTiempo[i] = tiempoActual;

                        tiempoUltimoSolCielo = tiempoActual;
                        break;
                    }
                }
            }
            
            //Este bucle es para que soles del cielo se muevan
            for (int i = 0; i < SOLES_MAXIMOS; i++) {
                if ((solesPantalla[i] == 1) && (solesTipo[i] == SOL_CIELO)) {
                    solesY[i] += velocidadSol *GetFrameTime();
                }
            }

            //Verificamos si algun sol llego a su tiempo maximo para hacerlo despawnear
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
            //Primero comprobamos si ya paso el cooldown de generacion de zombies y si todavia quedan zombies por generar en la ronda
            if ((tiempoActual - tiempoUltimoZombie >= cooldownZombie) && (rondaActiva == 1) && (zombiesGenerados < zombiesPorRonda)) {
                for (int i = 0; i < ZOMBIES_MAXIMOS; i++) {
                    if (zombies[i].vivo == MUERTO) {
                        int filaZombie = GetRandomValue(0, filas-1); //Genera una fila aleatoria para el spawn del zombie
                        zombies[i].x = pxDistanciaX + anchoJardin - anchoZombie;
                        zombies[i].y = pxDistanciaY + filaZombie * altoCasilla + ((altoCasilla - altoZombie) / 2);
                        zombies[i].tiempoUltimoAtaque = tiempoActual;

                        zombies[i].columnaAtaque = -1;
                        int tipoZombie = GetRandomValue (1,100); //Esto es para aleatorizar que tipo de zombie spawnea
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
        Este sub-bloque es para controlar el movimiento de los zombies, la deteccion de
        plantas en su camino y sus ataques
        */
            //Reinicia el contador para calcular cuantos zombies siguen vivos
            zombiesVivos = 0;
            for (int i = 0; i < ZOMBIES_MAXIMOS; i++) {
                if (zombies[i].vivo == VIVO) {
                    zombiesVivos++; //Cuenta cuantos zombies quedan vivos

                    //Si el zombie esta caminando entra a este if
                    if (zombies[i].estado == CAMINANDO) {
                        zombies[i].x -= zombies[i].velocidad * GetFrameTime(); //Movimiento de los zombies
                        int filaZombie = (zombies[i].y - pxDistanciaY) / altoCasilla;
                        int columnaZombie = (zombies[i].x - pxDistanciaX) / anchoCasilla;

                        //Comprobamos si llego a la casa
                        if (zombies[i].x <= pxDistanciaX - (anchoZombie / 2)) {
                            zombieCasa++; //Se suma al contador de zombies en la casa
                            zombies[i].vivo = MUERTO; //Se cambia su estado a muerto

                        //Si no llego a la casa, comprobamos si hay una planta en su camino
                        } else if ((filaZombie >= 0 && filaZombie < filas) && (columnaZombie >= 0 && columnaZombie < columnas)) {
                            if (vidaPlanta[filaZombie][columnaZombie] > 0) {
                                zombies[i].estado = ATACANDO; //Cambia su estado a atacando
                                zombies[i].columnaAtaque = columnaZombie;
                                zombies[i].filaAtaque = filaZombie;
                            }
                        }

                    //Si el zombie esta atacando a este if 
                    } else if (zombies[i].estado == ATACANDO) { 
                        //Si la planta es destruida se elimina del jardin y el zombie vuelve a caminar
                        if (vidaPlanta[zombies[i].filaAtaque][zombies[i].columnaAtaque] <= 0) {
                            jardin[zombies[i].filaAtaque][zombies[i].columnaAtaque] = VACIO;
                            vidaPlanta[zombies[i].filaAtaque][zombies[i].columnaAtaque] = 0;
                            zombies[i].estado = CAMINANDO;
                            zombies[i].columnaAtaque = -1;
                            zombies[i].filaAtaque = -1;
                        //Si la planta sigue viva y paso el cooldown del ataque del zombie, se le resta el daño del zombie
                        } else if (tiempoActual - zombies[i].tiempoUltimoAtaque >= zombies[i].cooldownAtaque) {
                            zombies[i].tiempoUltimoAtaque = tiempoActual;
                            vidaPlanta[zombies[i].filaAtaque][zombies[i].columnaAtaque] -= zombies[i].daño;
                        }
                    }
                }
            }






        /*
        Este bloque es para la generacion, movimiento y colisiones de los proyectiles
        */
            //Recorremos el jardin buscando lanzaguisantes que tengan zombies en la misma fila
            for (int i = 0; i < filas; i++) {
                for (int j = 0; j < columnas; j++) {
                    if (jardin[i][j] == LANZAGUISANTES) {
                        for (int k = 0; k < ZOMBIES_MAXIMOS; k++) {
                            //Si detecta un zombie, dispara
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

            //Actualizamos la posicion de los proyectiles en cada frame
            for (int i = 0; i < PROYECTILES_MAXIMOS; i++) {
                if (proyectiles[i].vivo == VIVO) {
                    proyectiles[i].x += proyectiles[i].velocidad * GetFrameTime();
                }
            }
            
            //Comprobamos si los proyectiles impactan a algun zombie o si salen del jardin
            for (int i = 0; i < PROYECTILES_MAXIMOS; i++) {
                if (proyectiles[i].vivo == VIVO) {
                    if (proyectiles[i].x >= pxDistanciaX + anchoJardin) {
                        proyectiles[i].vivo = MUERTO;
                    } else {
                        for (int j = 0; j < ZOMBIES_MAXIMOS; j++) {
                            if (zombies[j].vivo == VIVO) {
                                if (CheckCollisionCircleRec((Vector2){proyectiles[i].x,proyectiles[i].y}, proyectilesRadio, (Rectangle){zombies[j].x, zombies[j].y, anchoZombie, altoZombie})) {
                                    zombies[j].vida -= proyectiles[i].daño; //Restamos el daño del proyectil a la vida del zombie
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
        Este bloque es para calcular el avance de las rondas, la condicion de victoria
        y derrota, y el aumento progresivo de la dificultad
        */
            //Comprobamos si los zombies que llegaron a la casa alcanzaron el limite
            if (zombieCasa >= vidaCasa) {
                estadoJuego = LOSE; //El jugador pierde

            //Comprobamos si no quedan mas zombies por generar en la ronda, y ademas no queda ninguno vivo
            } else if ((rondaActiva == 1) && (zombiesVivos == 0) && (zombiesGenerados >= zombiesPorRonda)) {
                if (rondaActual == cantidadRondas) { //Verificamos si la ronda es igual a la cantidad maxima de rondas
                    estadoJuego = WIN; //El jugador gana
                } else { //Si no, se avanza a la siguiente ronda
                    rondaActiva = 0;
                    rondaActual++;
                    tiempoInicioGracia = tiempoActual;

                    //Aumentamos la dificultad de la siguiente ronda:
                    if (zombiesPorRonda < 30)
                        zombiesPorRonda += 2;
                    if (cooldownZombie < 2.0)
                        cooldownZombie -= 0.5;
                    if (probabilidadNormal < 30)
                        probabilidadNormal -= 5;

                }

            //Si no hay ronda activa, comprobamos si termino el tiempo de gracia
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