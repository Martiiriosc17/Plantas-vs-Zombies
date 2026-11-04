#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

//Definiciones para la matriz del jardin
#define VACIO 0
#define GIRASOL 1
#define LANZAGUISANTES 2
#define NUEZ 3

#define SOL_GIRASOL 1
#define SOL_CIELO 2

#define SOLES_MAXIMOS 50 //Cantidad maxima de soles en pantalla

//Aqui estan todas las funciones de raylib: https://www.raylib.com/cheatsheet/cheatsheet.html

int main(void) {
    // Primero ajustamos el tamaño del patio
    int filas = 5;
    int columnas = 9;

    // Dimensiones de la ventana donde se abre el juego
    int pxAncho = 1440; //1440
    int pxAlto = 810; // 810

    // Dimensiones del jardin
    int pxDistanciaX = 224; //Distancia de izq a dd empieza el jardin
    int pxDistanciaY = 81; //Distancia de arriba a dd empieza el jardin
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
    int solesTipo[SOLES_MAXIMOS]; //Cielo o girasol
    int solesPantalla[SOLES_MAXIMOS]; //Valor booleano de si en las coords hay un sol
    for (int i = 0; i < SOLES_MAXIMOS; i++) {
        solesX[i] = 0;
        solesY[i] = 0;
        solesTipo[i] = 0;
        solesPantalla[i] = 0;
    }
    double tiempoUltimoSol = 0; //Se ocupan double xq los usa raylib y para mayor precision
    double cooldownSol = 5.0;

    float velocidadSol = 40.0;
    double tiempoUltimoSolCielo = 0;
    double cooldownSolCielo = 7.0;





    // Reserva de memoria para la matriz
    int **jardin = malloc(filas * sizeof(int *)); //Primero dejamos espacio para las filas
    if (jardin == NULL) { // Verificamos que se pueda reservar la memoria
        printf("Error al reservar memoria.\n");
        return 1;
    }

    for (int i = 0; i < filas; i++) {
        jardin[i] = malloc(columnas * sizeof(int)); //Ahora dejamos espacio para las columnas

        //Verificamos que se pueda reservar la memoria
        if (jardin[i] == NULL) {
            printf("Error al reservar memoria.\n");
            for (int j = 0; j < i; j++) //Si no se puede liberamos los espacios que ya fueron ocupados hasta el error
                free(jardin[j]);
            free(jardin);

            return 1; // Significa que el programa fallo y rompe el main()
        }
    }



    // Inicializamos todas las casillas en 0
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            jardin[i][j] = VACIO;
        }
    }



    InitWindow(pxAncho, pxAlto, "Plantas vs Zombies (Probanding)"); //Se crea la ventana
    SetTargetFPS(67); //FPS de la ventana

    // Como las dimensiones son configurables, calculamos el tamaño de cada casilla
    float anchoCasilla = (float)anchoJardin/columnas;
    float altoCasilla = (float)altoJardin/filas;






    int filaSeleccionada = -1;
    int columnaSeleccionada = -1;
    int plantaSeleccionada = VACIO;

    //Bucle de la pestaña abierta, esto es para que se ejecute el juego en si
    while (!WindowShouldClose()) { //Mientras no (!) se tenga que cerrar la ventana)
    

    /*
    Este primer bloque es de la logica de los clicks, detecta donde esta el mouse y donde se hace el click
    para despues decidir si corresponde a una casilla del jardin, a una de las plantas o a un lugar sin nada    
    */
        Vector2 posMouse = GetMousePosition(); //devuelve la posicion x e y (con posMouse.x y posMouse.y se accede a c/u)
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { //Verificamos que el click esta dentro del jardin
            int solRecogido = 0;
            for (int i = 0; i < SOLES_MAXIMOS; i++) { //Verificamos si hay un sol y hace click en el
                if (CheckCollisionPointCircle(posMouse,(Vector2){.x=solesX[i],.y=solesY[i]},solesRadio) && (solesPantalla[i] == 1)) {
                    soles += 25;
                    solesPantalla[i] = 0;
                    solRecogido = 1;
                    solesTipo[i] = 0;
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
    Este segundo bloque es para el spawn d soles
    */
        double tiempoActual = GetTime();
        if (tiempoActual - tiempoUltimoSol >= cooldownSol) {
            for (int i = 0; i < filas; i++) {
                for (int j = 0; j < columnas; j++) {
                    if (jardin[i][j] == GIRASOL) {
                        for (int k = 0; k < SOLES_MAXIMOS; k++) {
                            if (solesPantalla[k] == 0) {
                                solesPantalla[k] = 1;
                                solesTipo[k] = SOL_GIRASOL;
                                solesX[k] = pxDistanciaX + anchoCasilla / 2 + j * anchoCasilla;
                                solesY[k] = pxDistanciaY + altoCasilla / 2 + i * altoCasilla;

                                break;
                            }
                        }
                    }
                }
            }
            tiempoUltimoSol = tiempoActual;
        }

        if (tiempoActual - tiempoUltimoSolCielo >= cooldownSolCielo) {
            for (int i = 0; i < SOLES_MAXIMOS; i++) {
                if (solesPantalla[i] == 0) {
                    solesPantalla[i] = 1;
                    solesTipo[i] = SOL_CIELO;
                    solesX[i] = GetRandomValue(224,1376); //Valores pueden cambiar segun coso
                    solesY[i] = pxDistanciaY - solesRadio;

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

    




        //Para dibujar todo |el fondo|
        BeginDrawing();
        ClearBackground(DARKBROWN);

        DrawFPS(10,10);
        DrawText("PLANTAS VS ZOMBIES (FRUNA EDITION)", 770, 35, 30, WHITE);


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


        EndDrawing();

    }
    CloseWindow();



    // Liberamos la memoria
    for (int i = 0; i < filas; i++)
        free(jardin[i]);

    free(jardin);

    return 0;
}
