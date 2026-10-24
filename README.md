# Plantas-vs-Zombies 
Tarea 1: Lectura de la entrada. En esta primera tarea se deberán leer al menos dos archivos, uno conteniendo la configuración inicial del juego (cantidad máxima de oleadas zombies, resistencia de los zombies, resistencia de la casa, frecuencia base de disparo de las plantas, frecuencia base de generación de soles, etc), y otro conteniendo el mapa/jardín donde se desarrollará el juego. Este segundo archivo deberá contener toda la información para generar el laberinto, incluyendo posiciones de obstáculos (en caso que se opte por incluir obstáculos). La forma en la que se almacenará todo queda como parte de la tarea, pero se recomienda el uso de matrices. La idea es que se escriba un programa que pueda generar patios al azar, el cual puede ser almacenada en un archivo y ser cargada al ejecutar el juego (Nota: Al momento de entregar el proyecto se deberá entregar un par de ejemplos de archivos de entrada.)

En resumen, esta tarea tendrá la siguiente entrada y salida:
Entrada: Nombre de dos archivos, uno de configuración y otro con el mapa/jardín
Salida: Para representar el laberinto usaremos matrices, en la cual se marcarán las celdas que contengan los elementos del juego (plantas, zombies, obstáculos, etc). Se recomienda usar consensos, como que una celda con 0 representa a una zona sin nada (pasto), un 1 indica el primer tipo de zombie, 2 el segundo tipo de zombie, etc.

Tarea 2: Lógica del juego. En esta tarea se debe implementar toda la lógica del juego, es decir, todo el movimiento de los zombies, efecto de las plantas, generación de soles, etc.

En resumen, esta tarea tendrá la siguiente entrada y salida:
Entrada: La entrada será la salida de la tarea 1. Se puede desarrollar esta tarea en paralelo a la tarea 1 asumiendo una matriz fija con valores 0, 1, 2, etc (según el consenso).
Salida: La salida será una matriz como la matriz de entrada, pero con los elementos modificados según las reglas del juego (Ej, zombies desplazados).

Tarea 3: Visualización del mapa/jardín. En esta tarea deberán implementar una interfaz gráfica para visualizar el mapa/jardín. Para realizarlo podrán utilizar ASCII art, el cual, mezclado con el uso de colores, puede proveer una buena alternativa para visualizar los objetos (ver en la sección de mejoras para más alternativas). Para esta tarea se constará son lo siguiente:
- Entrada: La entrada será la salida de la tarea 1 o tarea 2. Para avanzar de manera independiente, se puede asumir una matriz fija con valores 0, 1, 2, etc (según el consenso).
- Salida: Una representación visual del jardín y los objetos en él.
Mejoras a la implementación base
Todo lo descrito en las secciones anteriores define la versión base del juego, el que permite obtener una nota máxima de 4.9 (ver rúbrica). Por sobre la implementación base mostrada en el diagrama anterior, se pueden implementar los siguientes paquetes de mejora

Paquete 1: Almacenamiento y cambio de estados. En este paquete de mejoras se incluyen todas aquellas mejoras que permitan el almacenamiento del estado actual de la matriz, así como cambios en su estado. Algunos ejemplos de nuevas funcionalidades son
Guardar en un archivo el estado actual de la matriz al presionar una tecla en particular.
Cargar un estado guardado de la matriz. Esta funcionalidad se complementa con la anterior
Aumentar el tamaño de la matriz (ojo, que esto conlleva ajustar de manera adecuada la velocidad de generación de soles, ya que de otra manera se generarían problemas defensivos al no poder cubrir todas las zonas si la matriz es muy grande)
Otras funcionalidades similares
Paquete 2: Mejora en la usabilidad/jugabilidad. Este paquete de mejoras busca hacer más interactivo el juego. Ejemplo de nuevas funcionalidades son
Añadir un nuevo tipo objeto. Estos objetos pueden tener funcionalidades que los/las estudiantes decidan
Nuevos personajes: añadir nuevos tipos de plantas y/o zombies
Llevar un contador de disparos, oleadas transcurridas, etc.
Cambios en el comportamiento o poderes de las plantas y/o zombies (ej, zombies que se puedan mover en diferentes direcciones al encontrarse con obstáctulos)
Otras funcionalidades similares
Paquete 3: Mejoras en la visualización. Este paquete de mejoras busca hacer más atractiva la interfaz desarrollada. Alguna posibles mejoras son
Usar bibliotecas gráficas como SDL2, SDL3, Raylib, etc. Al menos una de ellas será enseñada por medio de una cápsula o en laboratorio. Puedes revisar proyectos de años anteriores para ver ejemplos de interfaces gráficas con SDL2 (linkLinks to an external site.)
Añadir algún efecto visual o de sonido.
Otras funcionalidades similares
Paquete 4: Mejoras personalizadas. Este paquete incluye cualquier mejora propuesta por el equipo de trabajo que no haya sido considerada en los paquetes de mejora previos
