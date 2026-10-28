#include <stdio.h>
#include <stdlib.h>   /* para malloc y free */

/* Crea una matriz de filas x columnas usando memoria dinamica */
int **crear_matriz(int filas, int columnas)
{
    int i;
    int **matriz;

    /* 1) Un arreglo de punteros: uno por cada fila */
    matriz = malloc(filas * sizeof(int *));
    if (matriz == NULL) {
        return NULL;
    }

    /* 2) Cada fila: un bloque de 'columnas' enteros */
    for (i = 0; i < filas; i++) {
        matriz[i] = malloc(columnas * sizeof(int));
    }

    return matriz;
}

/* Devuelve la memoria: primero cada fila, luego el arreglo de punteros */
void liberar_matriz(int **matriz, int filas)
{
    int i;
    for (i = 0; i < filas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

int main(void)
{
    FILE *archivo;
    int filas, columnas;
    int i, j;
    int **jardin;

    archivo = fopen("jardin.txt", "r");
    if (archivo == NULL) {
        printf("No se pudo abrir jardin.txt\n");
        return 1;
    }

    if (fscanf(archivo, "%d %d", &filas, &columnas) != 2) {
        printf("Error: la primera linea de jardin.txt no tiene dos numeros\n");
        fclose(archivo);
        return 1;
    }
    printf("El jardin tiene %d filas y %d columnas\n", filas, columnas);

    /* Ahora que sabemos el tamano, pedimos la memoria */
    jardin = crear_matriz(filas, columnas);
    if (jardin == NULL) {
        printf("Error: no hay memoria para el jardin\n");
        fclose(archivo);
        return 1;
    }

    /* Leer cada numero y guardarlo en su celda */
    for (i = 0; i < filas; i++) {
        for (j = 0; j < columnas; j++) {
            fscanf(archivo, "%d", &jardin[i][j]);
        }
    }

    fclose(archivo);   /* desde aqui el archivo ya no se usa */

    /* Mostrar lo que quedo guardado en la matriz */
    for (i = 0; i < filas; i++) {
        for (j = 0; j < columnas; j++) {
            printf("%d ", jardin[i][j]);
        }
        printf("\n");
    }

    liberar_matriz(jardin, filas);
    return 0;
}