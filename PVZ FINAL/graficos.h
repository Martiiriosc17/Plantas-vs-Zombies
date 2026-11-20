#ifndef GRAFICOS_H
#define GRAFICOS_H
#include "juego.h"

#define DURACION_EXPLOSION 0.6 // Cuanto dura la animacion de la explosion de la cereza (en segundos)

//Texturas varias:
extern Texture2D texturaSol;
extern Texture2D texturaCorazon;
extern Texture2D texturaCorazonNegro;
extern Texture2D texturaObstaculo;

//Texturas de las plantas
extern Texture2D texturaGirasol;
extern Texture2D texturaLanzaguisantes;
extern Texture2D texturaNuez;
extern Texture2D texturaMetralla;
extern Texture2D texturaCongeladora;
extern Texture2D texturaCereza;
extern Texture2D texturaPala;

//Texturas de los zombies
extern Texture2D texturaZombieNormal;
extern Texture2D texturaZombieFlag;
extern Texture2D texturaZombieBalde;

//Texturas de los proyectiles
extern Texture2D texturaGuisante;
extern Texture2D texturaGuisanteHelado;


//Funciones de dibujo
void textoCentrado(const char *texto, int centroX, int y, int tam, Color color);
float minimo(float a, float b);
Rectangle rectCasilla(int fila, int columna);
void dibujarBarraVida(float x, float y, float ancho, float vida, float vidaMax);
void dibujarCorazon(Vector2 c, float r, int perdido);
void dibujarIconoSol(Vector2 centro, float radio);
void dibujarGirasol(Rectangle r, float alfa);
void dibujarLanzador(Rectangle r, int tipo, float alfa);
void dibujarNuez(Rectangle r, int vida, float alfa);
void dibujarCerezaEn(Rectangle r, float escala, float alfa);
void dibujarPala(Rectangle r, float alfa);
void dibujarObstaculo(Rectangle r);
void dibujarPlanta(int tipo, Rectangle r, float alfa);
void dibujarUnZombie(Zombie z, float w, float h);

//Cargar y liberar las texturas
void cargarTexturas(void);
void liberarTexturas(void);

//Elementos de la interfaz y del juego
void dibujarHUD(void); 
void dibujarContadorSoles(void); 
void dibujarMenuPlantas(void); 
void dibujarJardin(void);
void dibujarCereza(void);  
void dibujarVistaPrevia(void);
void dibujarZombies(void);
void dibujarProyectiles(void); 
void dibujarSoles(void);
void dibujarAvisoOleada(void);
void dibujarCursorPala(void);

//Menu y pantallas finales
Rectangle botonMenu(int indice);
void dibujarMenu(void);
void dibujarPantallaFinal(int gano); // gano = 1 victoria, 0 derrota
#endif
