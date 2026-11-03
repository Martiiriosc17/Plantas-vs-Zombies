#include <stdio.h>

int main(void) {
    FILE *archivo; // Así representa C un archivo abierto (va avanzando por el texto)
    int filas,columnas,valor;

    archivo = fopen("Jardin.txt","r"); // La "r" es de reading, solo para leer el archivo (no trabajar con el)

    if (archivo == NULL) { // Verifica que el archivo existe y que no hayan bugs después
        printf("No se pudo abrir el archivo Jardin.txt\n");
        return 1;
    }

    if (fscanf(archivo,"%d %d", &filas,&columnas) !=2) { // Asegura que el archivo tenga 2 primeros valores (filas y columnas)
        printf("Error, la primera linea de Jardin.txt no tiene 2 números\n");
        fclose(archivo);
        return 1;
    }

    /*
    fscanf (archivo, "%d %d" , &filas,&columnas); // Para guardar los valores de filas y columnas
    printf("El jardín tiene %d filas y %d columnas\n",filas,columnas);
    fclose(archivo);
    */

    for (int i=0;i<filas;i++) {
        for (int j=0;j<columnas;j++) {
            fscanf (archivo,"%d",&valor);
            printf("%d ",valor);
        }
        printf("\n");
    }
    fclose(archivo);

    return 0;
}
