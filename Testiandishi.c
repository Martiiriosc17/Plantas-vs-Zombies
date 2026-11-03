#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

//Aqui estan todas las funciones de raylib: https://www.raylib.com/cheatsheet/cheatsheet.html

int main(void) {
    // Primero ajustamos el tamaño del patio
    int filas = 5;
    int columnas = 9;

    // Dimensiones de la ventana donde se abre el juego
    int pxAncho = 1440;
    int pxAlto = 810;

    // Dimensiones del jardin
    int pxDistanciaX = 144; //Distancia de izq a dd empieza el jardin
    int pxDistanciaY = 81; //Distancia de arriba a dd empieza el jardin
    int anchoJardin = 1152; //Ancho total del jardin
    int altoJardin = 648; //Alto total del jardin



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
            jardin[i][j] = 0;
        }
    }



    InitWindow(pxAncho, pxAlto, "Plantas vs Zombies (Probanding)"); //Se crea la ventana
    SetTargetFPS(60); //FPS de la ventana

    // Como las dimensiones son configurables, calculamos el tamaño de cada casilla
    float anchoCasilla = (float)anchoJardin/columnas;
    float altoCasilla = (float)altoJardin/filas;

    // Guardar click del usuario, inicializado fuera de la matriz
    int filaSeleccionada = -1;
    int columnaSeleccionada = -1;



    //Bucle de la pestaña abierta, esto es para que se ejecute el juego en si
    while (!WindowShouldClose()) { //Mientras no (!) se tenga que cerrar la ventana)
        Vector2 posMouse = GetMousePosition(); //devuelve la posicion x e y (con posMouse.x y posMouse.y se accede a c/u)

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { //Verificamos que el click esta dentro del jardin
            if ((posMouse.x>pxDistanciaX) && (posMouse.x<(pxDistanciaX)+anchoJardin) && (posMouse.y>pxDistanciaY) && (posMouse.y<(pxDistanciaY)+altoJardin)) {
                int casillax = ((posMouse.x - pxDistanciaX) / anchoCasilla);
                columnaSeleccionada = casillax;
                int casillay = ((posMouse.y - pxDistanciaY) / altoCasilla);
                filaSeleccionada = casillay;

                printf("Fila: %d | Columna: %d\n", casillay, casillax);
            } else { //Si esta afuera se reinician sus valores
                filaSeleccionada = -1;
                columnaSeleccionada = -1;
            }
        }



        //Para dibujar todo |el fondo|
        BeginDrawing();
        ClearBackground(DARKBROWN);

        DrawFPS(10,10);
        DrawText("PLANTAS VS ZOMBIES (FRUNA EDITION)", 770, 35, 30, WHITE);

        // Dibujamos la cuadricula
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

                DrawRectangleLines(x, y,anchoCasilla, altoCasilla, BLACK);
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