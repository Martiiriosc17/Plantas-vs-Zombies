# Plantas-vs-Zombies 
# Tarea 1: Lectura de la entrada. En esta primera tarea se deberán leer al menos dos archivos, uno conteniendo la configuración inicial del juego (cantidad máxima de oleadas zombies, resistencia de los zombies, resistencia de la casa, frecuencia base de disparo de las plantas, frecuencia base de generación de soles, etc), y otro conteniendo el mapa/jardín donde se desarrollará el juego. Este segundo archivo deberá contener toda la información para generar el laberinto, incluyendo posiciones de obstáculos (en caso que se opte por incluir obstáculos). La forma en la que se almacenará todo queda como parte de la tarea, pero se recomienda el uso de matrices. La idea es que se escriba un programa que pueda generar patios al azar, el cual puede ser almacenada en un archivo y ser cargada al ejecutar el juego (Nota: Al momento de entregar el proyecto se deberá entregar un par de ejemplos de archivos de entrada.)

# En resumen, esta tarea tendrá la siguiente entrada y salida:
# Entrada: Nombre de dos archivos, uno de configuración y otro con el mapa/jardín
# Salida: Para representar el laberinto usaremos matrices, en la cual se marcarán las celdas que contengan los elementos del juego (plantas, zombies, obstáculos, etc). Se recomienda usar consensos, como que una celda con 0 representa a una zona sin nada (pasto), un 1 indica el primer tipo de zombie, 2 el segundo tipo de zombie, etc.

# Tarea 2: Lógica del juego. En esta tarea se debe implementar toda la lógica del juego, es decir, todo el movimiento de los zombies, efecto de las plantas, generación de soles, etc.

# En resumen, esta tarea tendrá la siguiente entrada y salida:
# Entrada: La entrada será la salida de la tarea 1. Se puede desarrollar esta tarea en paralelo a la tarea 1 asumiendo una matriz fija con valores 0, 1, 2, etc (según el consenso).
# Salida: La salida será una matriz como la matriz de entrada, pero con los elementos modificados según las reglas del juego (Ej, zombies desplazados).

# Tarea 3: Visualización del mapa/jardín. En esta tarea deberán implementar una interfaz gráfica para visualizar el mapa/jardín. Para realizarlo podrán utilizar ASCII art, el cual, mezclado con el uso de colores, puede proveer una buena alternativa para visualizar los objetos (ver en la sección de mejoras para más alternativas). Para esta tarea se constará son lo siguiente:
# - Entrada: La entrada será la salida de la tarea 1 o tarea 2. Para avanzar de manera independiente, se puede asumir una matriz fija con valores 0, 1, 2, etc (según el consenso).
# - Salida: Una representación visual del jardín y los objetos en él.
