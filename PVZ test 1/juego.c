#include "juego.h"
#include <stdlib.h>
#include <stdio.h>

int cargarConfig(const char *nombreArchivo) {
    FILE *Configuracion = fopen(nombreArchivo, "r");

    if (Configuracion == NULL) {
        printf("No se pudo abrir el archivo Config.txt\n");
        return 0;
    }

    int auxVidaCasa;
    int auxCantidadRondas;
    int auxZombiesPorRonda;
    int auxCantidadObstaculos;
    double auxDuracionSol;
    double auxCooldownZombie;
    double auxCooldownSol;
    double auxCooldownSolCielo;

    if (fscanf(Configuracion, "%d %d %d %d %lf %lf %lf %lf", &auxVidaCasa, &auxCantidadRondas, &auxZombiesPorRonda, &auxCantidadObstaculos, &auxDuracionSol, &auxCooldownZombie, &auxCooldownSol, &auxCooldownSolCielo) != 8) {
        printf("Error al leer la configuracion inicial en Config.txt\n");
        fclose(Configuracion);
        return 0;
    }

    if ((auxVidaCasa <= 0) || (auxCantidadRondas <= 0) || (auxZombiesPorRonda <= 0) || (auxCantidadObstaculos < 0) || (auxDuracionSol <= 0) || (auxCooldownZombie <= 0) || (auxCooldownSol <= 0) || (auxCooldownSolCielo <= 0)) {
        printf("Error de los valores (deben ser positivos)\n");
        fclose(Configuracion);
        return 0;
    }

    vidaCasa = auxVidaCasa;
    cantidadRondas = auxCantidadRondas;
    zombiesPorRonda = auxZombiesPorRonda;
    cantidadObstaculos = auxCantidadObstaculos;
    duracionSol = auxDuracionSol;
    cooldownZombie = auxCooldownZombie;
    cooldownSol = auxCooldownSol;
    cooldownSolCielo = auxCooldownSolCielo;

    fclose(Configuracion);
    return 1;
}

int cargarJardin(const char *nombreArchivo) {
    FILE *Jardin = fopen(nombreArchivo, "r");

    if (Jardin == NULL) {
        printf("No se pudo abrir Jardin.txt\n");
        return 0;
    }

    int auxFilas;
    int auxColumnas;

    if (fscanf(Jardin, "%d %d", &auxFilas, &auxColumnas) != 2) {
        printf("Error, la primera linea de Jardin.txt no tiene 2 numeros\n");
        fclose(Jardin);
        return 0;
    }

    if ((auxFilas <= 0) || (auxFilas >= 20) || (auxColumnas <= 0) || (auxColumnas >= 20)) {
        printf("Las dimensiones del jardin deben ser mayor que cero\n");
        fclose(Jardin);
        return 0;
    }

    filas = auxFilas;
    columnas = auxColumnas;

    if (inicializarMatrices() == 0) {
        printf("Error al reservar memoria para las matrices\n");
        fclose(Jardin);
        return 0;
    }

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            if (fscanf(Jardin, "%d", &jardin[i][j]) != 1) {
                printf("Error al leer una casilla del jardin\n");
                liberarMatrices();
                fclose(Jardin);
                return 0;
            }
            if ((jardin[i][j] != VACIO) && (jardin[i][j] != OBSTACULO)) {
                printf("Error: Elemen##to invalido en el jardin\n");
                liberarMatrices();
                fclose(Jardin);
                return 0;
            }
        }
    }
    fclose(Jardin);
    return 1;
}





int generarObstaculos(void) {
    int obstaculosGenerados = 0;
    int casillasDisponibles = 0;

    //Hay que recorrer el bucle por si en jardin.txt ya hay algun obstaculo
    for (int i = 0; i < filas; i++) {
        for (int j = MARGEN_OBSTACULO; j < columnas; j++) {
            if (jardin[i][j] == VACIO) {
                casillasDisponibles++;
            }
        }
    }
    
    if (cantidadObstaculos > casillasDisponibles) {
        printf("No hay suficientes casillas disponibles para los obstaculos\n");
        return 0;
    }

    while (obstaculosGenerados < cantidadObstaculos) {
        int filaRandom = GetRandomValue(0, filas - 1);
        int columnaRandom = GetRandomValue(MARGEN_OBSTACULO, columnas - 1);

        if (jardin[filaRandom][columnaRandom] == VACIO) {
            jardin[filaRandom][columnaRandom] = OBSTACULO;
            obstaculosGenerados++;
        }
    }
    return 1;
}

  










int **crearMatrizInt(int filas, int columnas) {
    int **matriz = malloc(filas * sizeof(int *)); //Reserva memoria para los punteros de las filas
    if (matriz == NULL) { //Comprueba que se pueda reservar la memoria
        return NULL;
    }
    for (int i = 0;i < filas; i++) { //Reserva memoria para las columnas de cada fila
        matriz[i] = malloc(columnas * sizeof(int));
        if (matriz[i] == NULL) { //Si no puede reservarla en algun punto, vacia todo lo ya reservado
            for (int j = 0; j < i; j++) {
                free(matriz[j]);
            }
            free(matriz);
            return NULL;
        }
    }
return matriz;
}



double **crearMatrizDouble(int filas, int columnas) {
    double **matriz = malloc(filas * sizeof(double *));
    if (matriz == NULL) {
        return NULL;
    }
    for (int i = 0;i < filas; i++) { 
        matriz[i] = malloc(columnas * sizeof(double));
        if (matriz[i] == NULL) { 
            for (int j = 0; j < i; j++) {
                free(matriz[j]);
            }
            free(matriz);
            return NULL;
        }
    }
return matriz;
}



//Esta funcion es para inicializar una matriz en memoria dinamica
int inicializarMatrices(void) {
    jardin = crearMatrizInt(filas, columnas);
    tiempoUltimoSolGirasol = crearMatrizDouble(filas, columnas);
    tiempoUltimoDisparo = crearMatrizDouble(filas, columnas);
    vidaPlanta = crearMatrizInt(filas,columnas);

    //Verifica que las matrices se hayan reservado sin errores
    if ((jardin == NULL) || (tiempoUltimoSolGirasol == NULL) || (tiempoUltimoDisparo == NULL) || (vidaPlanta == NULL)) {
        liberarMatrices();
        return 0;
    }

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            jardin[i][j] = VACIO;
            tiempoUltimoSolGirasol[i][j] = 0;
            tiempoUltimoDisparo[i][j] = 0;
            vidaPlanta[i][j] = 0;
        }
    }
return 1;
}



//Esta funcion es para liberar el espacio de las matrices
void liberarMatrices(void) {
    if (jardin != NULL) {
        for (int i = 0; i < filas; i++) {
            free(jardin[i]);
        }
        free(jardin);
        jardin = NULL;
    }
    if (tiempoUltimoSolGirasol != NULL) {
        for (int i = 0; i < filas; i++) {
            free(tiempoUltimoSolGirasol[i]);
        }
        free(tiempoUltimoSolGirasol);
        tiempoUltimoSolGirasol = NULL;
    }
    if (tiempoUltimoDisparo != NULL) {
        for (int i = 0; i < filas; i++) {
            free(tiempoUltimoDisparo[i]);
        }
        free(tiempoUltimoDisparo);
        tiempoUltimoDisparo = NULL;
    }
    if (vidaPlanta != NULL) {
        for (int i = 0; i < filas; i++) {
            free(vidaPlanta[i]);
        }
        free(vidaPlanta);
        vidaPlanta = NULL;
    }
}



//Esta funcion controla la generacion, movimiento y desaparicion de los soles
void actualizarSoles(double tiempoActual) {
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
}



//Funcion para generar los zombies
void generarZombies(double tiempoActual) {
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
                    zombies[i].dano = 10;
                    zombies[i].cooldownAtaque = 1.5;
                    
                } else {
                    zombies[i].tipo = FLAG;
                    zombies[i].velocidad = 60;
                    zombies[i].vida = 150;
                    zombies[i].estado = CAMINANDO;
                    zombies[i].dano = 10;
                    zombies[i].cooldownAtaque = 1.5;
                }
                zombies[i].vivo = VIVO;
                zombiesGenerados++;
                break;
            }
        }
        tiempoUltimoZombie = tiempoActual;
    }
}



//Funcion para controlar el movimiento, ataque y deteccion de plantas de los zombies
void actualizarZombies(double tiempoActual) {
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
            }
            //Si el zombie esta atacando a este if 
            if (zombies[i].estado == ATACANDO) { 
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
                    vidaPlanta[zombies[i].filaAtaque][zombies[i].columnaAtaque] -= zombies[i].dano;
                }
            }
        }
    }
}



//Funcion para controlar la generacion, colision y movimiento de los proyectiles
void actualizarProyectiles(double tiempoActual) {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {

            if (jardin[i][j] == LANZAGUISANTES) {
                for (int k = 0; k < ZOMBIES_MAXIMOS; k++) {
                    //Si detecta un zombie, dispara
                    if (zombies[k].vivo == VIVO) {
                        float pxSuperior = pxDistanciaY + i * altoCasilla;
                        float pxInferior = pxSuperior + altoCasilla;
                        if ((zombies[k].y >= pxSuperior) && (zombies[k].y < pxInferior) && (tiempoActual - tiempoUltimoDisparo[i][j] >= cooldownLanzaguisantes)) {
                            for (int l = 0; l < PROYECTILES_MAXIMOS; l++) {
                                if ((proyectiles[l].vivo == MUERTO) && (zombies[k].x > pxDistanciaX + j * anchoCasilla)) {
                                    proyectiles[l].vivo = VIVO;
                                    proyectiles[l].dano = 10;
                                    proyectiles[l].velocidad = 350;
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

            } else if (jardin[i][j] == METRALLA) {
                for (int k = 0; k < ZOMBIES_MAXIMOS; k++) {
                    //Si detecta un zombie, dispara
                    if (zombies[k].vivo == VIVO) {
                        float pxSuperior = pxDistanciaY + i * altoCasilla;
                        float pxInferior = pxSuperior + altoCasilla;
                        if ((zombies[k].y >= pxSuperior) && (zombies[k].y < pxInferior) && (tiempoActual - tiempoUltimoDisparo[i][j] >= cooldownMetralla)) {
                            for (int l = 0; l < PROYECTILES_MAXIMOS; l++) {
                                if ((proyectiles[l].vivo == MUERTO) && (zombies[k].x > pxDistanciaX + j * anchoCasilla)) {
                                    proyectiles[l].vivo = VIVO;
                                    proyectiles[l].dano = 10;
                                    proyectiles[l].velocidad = 350;
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
                            zombies[j].vida -= proyectiles[i].dano; //Restamos el daño del proyectil a la vida del zombie
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
}



//Para actualizar las rondas, y comprobar el estado de victoria y derrota
void actualizarRondas(double tiempoActual) {
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
            if (cooldownZombie > 2.0)
                cooldownZombie -= 0.5;
            if (probabilidadNormal > 30)
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



//Funcion para procesar los clicks
void procesarClicks(Vector2 posMouse) {
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
                if ((plantaSeleccionada == PALA) && (jardin[filaSeleccionada][columnaSeleccionada] != VACIO) && (jardin[filaSeleccionada][columnaSeleccionada] != OBSTACULO))  {
                    jardin[filaSeleccionada][columnaSeleccionada] = VACIO;
                    vidaPlanta[filaSeleccionada][columnaSeleccionada] = 0;
                    tiempoUltimoSolGirasol[filaSeleccionada][columnaSeleccionada] = 0;
                    tiempoUltimoDisparo[filaSeleccionada][columnaSeleccionada] = 0;
                    plantaSeleccionada = VACIO; 

                } else if ((plantaSeleccionada != VACIO) && (jardin[filaSeleccionada][columnaSeleccionada] == VACIO)){
                    //Se asigna el costo correspondiente segun el tipo de planta seleccionada
                    int costo = 0;
                    if (plantaSeleccionada == GIRASOL) {
                        costo = costoGirasol;
                    } else if (plantaSeleccionada == LANZAGUISANTES) {
                        costo = costoLanzaguisantes;
                    } else if (plantaSeleccionada == NUEZ) {
                        costo = costoNuez;
                    } else if (plantaSeleccionada == METRALLA) {
                        costo = costoMetralla;
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
            } else if ((posMouse.x>=64) && (posMouse.x<=204) && (posMouse.y>=140) && (posMouse.y<=725)) {
                for (int i = 0; i < 7; i++) { //Determinamos que planta selecciono el usuario
                    if ((posMouse.y>=140+85*i) && (posMouse.y<=140+85*(i+1))) {
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
}