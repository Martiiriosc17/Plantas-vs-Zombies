# Makefile del proyecto PVZ
#
#Comandos:
#   make        Compila el juego
#   make clean  Elimina los ejecutables
#
# En Windows se utilizan las carpetas include y lib de Raylib.
# En Linux se enlazan las librerias del sistema.

# Compilador y opciones de compilacion
CC = gcc
CFLAGS = -Wall -Wextra

#Archivos que forman parte del proyecto
FUENTES = main.c juego.c graficos.c
CABECERAS = juego.h graficos.h

# Configuracion segun el sistema operativo
ifeq ($(OS),Windows_NT)
    #Rutas y bibliotecas necesarias para Raylib en Windows
    CFLAGS += -Iinclude
    LIBS = -Llib -lraylib -lgdi32 -lwinmm
    EJECUTABLE = juego.exe
else
    # Bibliotecas necesarias para Raylib en Linux
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EJECUTABLE = juego
endif

#Compila el juego cuando alguno de sus archivos cambia
$(EJECUTABLE): $(FUENTES) $(CABECERAS)
	$(CC) $(CFLAGS) $(FUENTES) -o $(EJECUTABLE) $(LIBS)

#Elimina los ejecutables para poder hacer una compilacion limpia
clean:
	rm -f juego juego.exe

# Indica que clean es una tarea y no un archivo
.PHONY: clean