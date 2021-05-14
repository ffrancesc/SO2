# Documento de seguimiento
Este documento explica el progreso realizado de la implementación de un **Screen Splasher en *zeos***.

## 1. Nivel alcanzado
El nivel 1, la gestión de las entradas de teclado y la creación/destrucción de pantallas.

## 2. Nuevas decisiones tomadas
La lista all_screens, que íbamos a usar para guardar todas las pantallas creadas por el sistema operativo, la hemos eliminado. No la usaremos porque al cambiar de pantalla sólo necesitamos las pantallas que ha creado el proceso en cuestión, y no todas las pantallas creadas en general.

En el task_struct de un proceso hemos añadido un array de enteros: `int used_screens[10]`. Lo usamos para detectar qué pantallas están activas y cuáles no. De esta manera, al buscar cuál es la siguiente pantalla en la que poner el foco, podemos detectar si hay pantallas que no se usan, ya sea porque no se han inicializado nunca o porque han sido eliminadas. Debido a esta modificación no necesitamos la variable `int screen_count`, que íbamos a usar para contar las pantallas creadas por el proceso y así no crear más de 10.

Todas las funciones y variables relacionadas con las pantallas están ahora en *devices*, y no en *sched* como habíamos planificado inicialmente.

## 3. Problemas con nuestro diseño
En el `struct screen` que representa la pantalla hemos añadido la variable `char *buffer`, la cual necesitamos para guardar el texto que ha escrito un proceso en una pantalla. De otra manera perderíamos el texto al cambiar de pantalla.

Hemos modificado nuestro enfoque de cómo un proceso escribe en la pantalla. Ahora, ya que tenemos la variable *buffer*, al hacer un *write* se escribe en el *buffer* de la pantalla que tiene el foco, y luego se actualiza el texto de la consola escribiendo en ella el contenido del *buffer* de dicha pantalla.


## 4. Listado de tareas actualizado
Creación de pantallas para un único proceso: HECHO (testing)
Cambios de pantalla para un único proceso: HECHO (testing)
Destrucción de pantallas para un único proceso: HECHO (testing)
Herencia de pantallas entre procesos: PENDIENTE
Mejora general de rendimiento *Copy-on-Write*: PENDIENTE