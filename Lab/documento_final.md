# Documento final
Este documento explica el estado final del desarrollo de la implementación de un **Screen Splasher en *zeos***.

## 1. Nivel alcanzado
Se ha alcanzado el nivel 2, con todas las funcionalidades relacionadas con la gestión de pantallas sin haber aprofundido en la mejora de rendimiento *Copy-on-Write*.

## 2. Nuevas decisiones tomadas
Hemos cambiado el tipo de la lista de pantallas disponibles por el proceso de `screen_struct` a puntero `screen_struct*`, de forma que las pantallas ya no se guardan en el `task_struct` de un proceso. Acorde a lo que comentamos, hemos declarados un array estático con **todas** las pantallas disponibles en `sched.c`, haciendo que el array de punteros del `task_struct` apunte a éstas.

También se han añadido los atributos `last_screen_id` y `focus_screen_id` para saber hasta que indice del array tenemos pantallas inicializadas y qué pantalla es la activa en cada momento respectivamente.

En la línea de lo que comentamos, hemos cambiado el comportamineto del `switch_screen` para implementar la funcionalidad de poder dar el foco a pantallas de procesos que no están activos en ese momento. Así pues la combinación de teclas `'SHIFT + TAB'` permite ciclar entre todas las pantallas inicializadas en el SO.

Finalmente para implementar la combinación de teclas `'SHIFT + TAB'` hemos tenido que usar dos variables (una para cada tecla) en vez de un `enum` con un solo valor. 

## 3. Problemas con nuestro diseño
Las pantallas están almacenadas en espacio del sistema sin protección. Además, el array que almacena todas las pantallas posiblemente esté sobredimensionado.

## 4. Listado de tareas actualizado

* Creación de pantallas para un único proceso: HECHO + TEST
* Cambios de pantalla para un único proceso: HECHO + TEST
* Destrucción de pantallas para un único proceso: HECHO + TEST
* Herencia de pantallas entre procesos: HECHO + TEST
* Mejora general de rendimiento *Copy-on-Write*: Inacabado