# Makefile - PVZ (FRUNA)
#
# Uso:  make        -> compila el juego
#       make clean  -> borra el ejecutable
#
# Funciona en Windows (con w64devkit) y en Linux.
# En Windows, las carpetas "include" y "lib" del zip de raylib
# (raylib-5.5_win64_mingw-w64.zip) deben estar dentro de esta carpeta.
#
# Ojo: las lineas que empiezan con $(CC) y rm deben empezar con TAB, no espacios.

CC = gcc
CFLAGS = -Wall -Wextra
FUENTES = main.c juego.c graficos.c
CABECERAS = juego.h graficos.h

ifeq ($(OS),Windows_NT)
    CFLAGS += -Iinclude
    LIBS = -Llib -lraylib -lgdi32 -lwinmm
    EJECUTABLE = juego.exe
else
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EJECUTABLE = juego
endif

$(EJECUTABLE): $(FUENTES) $(CABECERAS)
	$(CC) $(CFLAGS) $(FUENTES) -o $(EJECUTABLE) $(LIBS)

clean:
	rm -f juego juego.exe

.PHONY: clean
