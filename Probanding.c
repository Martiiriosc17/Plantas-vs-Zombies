#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> //Bibloteca para la gestion de tiempo y fechas

int main(void) { 
    srand(time(NULL)); //Inicializar la semilla de aleatoriedad para rand()

    //ARCHIVO 1: CONFIGURACION 
    FILE *Configuracion = fopen("Configuracion.txt", "r");

    if (Configuracion == NULL) {
        printf("No se pudo abrir el archivo Configuracion.txt\n");
        return 1; 
    }
    int max_oleadas;
    int resistencia_zombie1; //zombie1: normal  
    int resistencia_zombie2; //zombie2: bandera
    int resistencia_casa;
    int frecuencia_disparo;
    int frecuencia_girasoles;
    int frecuencia_cielo;

    if (fscanf(Configuracion, "%d %d %d %d %d %d %d", &max_oleadas, &resistencia_zombie1, &resistencia_zombie2, &resistencia_casa, &frecuencia_disparo, &frecuencia_girasoles, &frecuencia_cielo) != 5) { //leer los 5 valores de la configuracion inicial
        printf("Error al leer en la configuracion inicial en Configuracion\n");
        fclose(Configuracion);
        return 1; 
    } 

    fclose(Configuracion); //cerramos el archivo de configuracion


    //ARCHIVO 2: MAPA/JARDIN
    FILE *MapaJardin = fopen("Jardin.txt","r"); // Así representa C un archivo abierto (va avanzando por el texto). La "r" es de reading, solo para leer el archivo (no trabajar con el)

    if (MapaJardin == NULL) { // Verifica que el archivo existe y que no hayan bugs después
        printf("No se pudo abrir el archivo Jardin.txt\n");
        return 1;
    }
    
    int filas,columnas;

    if (fscanf(MapaJardin,"%d %d", &filas,&columnas) !=2) { // Asegura que el archivo tenga 2 primeros valores (filas y columnas)
        printf("Error, la primera linea de Jardin.txt no tiene 2 números\n");
        fclose(MapaJardin);
        return 1;
    }

    //Crear la matriz dinamica para guardar el mapa cargado del archivo
    int **jardin = malloc(filas * sizeof(int *)); //puntero llamado jardin, memoria Stack y memoria reservada malloc se ubica en Heap. Sizeof(int *): tamaño en bytes del puntero
    for (int i = 0; i < filas; i++){
        jardin[i] = malloc(columnas * sizeof(int)); //guarda ña direccion retornada por malloc en la posicion i. cada fila queda conectada a su propia franja de enteros
    } 

    //Leer la matriz desde el archivo y guardarla en la memoria
    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            fscanf(MapaJardin, "%d", &jardin[i][j]);
        }
    }
    fclose(MapaJardin); //Cerramos el archivo, luego de leerlo 

    //seleccion aleatoria de patio (0 = dia, 1 = noche)(como juego original) 
    int tipo_jardin = rand() % 2; //declaramos la variable tipo de jardin, rand -> aleatorio

    int cantidad_obstaculos = 4; //DEFINIR CANTIDAD DE OBTACULOS 
    int usados = 0; //obtaculos ya en la matriz

    while (usados < cantidad_obstaculos) { //si en la matriz hay menos obtaculos, que el total de obtaculos
        int f = rand() % filas; //se ponen obtaculos de forma aleatoria en alguna fila
        int c = rand() % columnas; // y columna

        if (jardin[f][c] == 0 && c != 0){ //un obtaculo solo puede estar posicionado en una casilla vacia y no en la primera columna 
            jardin[f][c] = 9; // 9 representa un obtaculo 
            usados++; 
        }
    } 

    printf("Maximo de oleadas:          %d\n", max_oleadas);
    printf("Resistencia Zombie normal:  %d\n", resistencia_zombie1);
    printf("Resistencia Zombie bandera: %d\n", resistencia_zombie2);
    printf("Resistencia de la casa:     %d\n", resistencia_casa);
    printf("Frecuencia de disparos:     %d\n", frecuencia_disparo); 
    printf("Frecuencia de girasoles:    %d\n", frecuencia_girasoles);
    printf("Frecuencia del cielo;       %d\n", frecuencia_cielo); 

    //Mostrar en que tipo de jardin se jugara 
    if (tipo_jardin == 0) {
        printf(" PATIO SELECCIONADO: DIA\n");
    } else {
        printf("PATIO SELECCIONADO: NOCHE\n");
    } 

    //Encabezados de columnas y filas
    printf("     ");
    for (int j = 0; j < columnas; j++) {
        printf(" %d ", j);
    } 
    printf("\n  +");
    for (int j = 0; j < columnas; j++) {
        printf("---");
    }
    printf("+\n"); 

    //Imprime filas con su numero de indice
    for (int i = 0; i < filas; i++) {
        printf(" %d |", i); //numero de fila
        for (int j = 0; j < columnas; j++) {
            printf(" %d ", jardin[i][j]);
        }
        printf("|\n"); //Borde derecho de cada fila
    } 
    //Borde inferior
    printf("   +");
    for (int j = 0; j < columnas; j++) {
        printf("---");
    } 
    printf("+\n\n"); 

    return 0;
} 

//NO ME DEJA EJECUTARLO, ME SALE QUE HAY UN ERROR EN EL ARCHIVO DE CONFIGURACION, DE LECTURA CREO
