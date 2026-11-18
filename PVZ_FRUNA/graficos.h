/*
 * graficos.h
 * Funciones que dibujan el juego con raylib.
 *
 * Regla de este modulo: estas funciones SOLO dibujan. No cambian el estado
 * del juego (no restan soles, no mueven zombies, no plantan). Leen las
 * variables globales declaradas en juego.h y las muestran en pantalla.
 * Toda la logica sigue en juego.c.
 */

#ifndef GRAFICOS_H
#define GRAFICOS_H

#include "juego.h"

/* ---------- Pantalla de juego ----------
   En main.c se llaman en este orden: lo que se dibuja despues queda encima. */

void dibujarHUD(void);            // Titulo, corazones de la casa y oleada actual (arriba)
void dibujarContadorSoles(void);  // Cuadro con los soles (arriba a la izquierda)
void dibujarPanelPlantas(void);   // Las 7 cartas: plantas, cereza y pala, con su costo
void dibujarJardin(void);         // Pasto, muro de la casa, obstaculos, plantas y casilla seleccionada
void dibujarCereza(void);         // Cereza a punto de explotar, su zona de dano y la explosion
void dibujarVistaPrevia(void);    // Planta semitransparente (o la pala) en la casilla bajo el mouse
void dibujarZombies(void);        // Zombies normales y flag (azulados si estan congelados)
void dibujarProyectiles(void);    // Guisantes normales, de metralla y de hielo
void dibujarSoles(void);          // Soles del cielo y de girasoles (al final, para que se vean)
void dibujarAvisoOleada(void);    // "La oleada X empieza en N s" durante el tiempo de gracia
void dibujarAyuda(void);          // Texto con los controles (abajo)
void dibujarCursorPala(void);     // Pala junto al mouse cuando esta seleccionada

/* ---------- Menu y pantallas finales ---------- */

// Rectangulo de cada boton del menu (0 = JUGAR, 1 = CONFIGURACION, 2 = SALIR).
// Lo usan dibujarMenu() y la logica del click en main.c, asi siempre coinciden.
Rectangle botonMenu(int indice);

void dibujarMenu(void);
void dibujarPantallaFinal(int gano); // gano = 1 victoria, 0 derrota

#endif
