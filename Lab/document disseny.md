# Documento de diseño

A continuación se detalla la especificación del diseño para la implementación de un **Screen Splasher en *zeos***.

Autores: *Sergi Gomà Cruells*  y *Francesc Salar Gagnach*

## 1. Nivel a alcanzar

Optamos al nivel 3.

## 2. Temas aplicables

* Interrupciones y excepciones
* Gestión de memoria a nivel usuario
* Gestión de memoria a nivel sistema
* Llamadas a sistema

## 3. Breve descripción de la implementación

Se optará por una implementación lo más conservadora y lo menos intrusiva posible: añadir únicamente lo estrictamente necesario para implementar las funcionalidades requeridas en los tres niveles del proyecto. Esto ayudará a reducir la complejidad de la implementación.
El plan de desarrollo a *grosso modo*:

  1. Creación de pantallas para un único proceso
  2. Cambios de pantalla para un único proceso
  3. Destrucción de pantallas para un único proceso
  4. Herencia de pantallas entre procesos.
  5. Mejora general de rendimiento *Copy-on-Write*

Para reducir y aislar las posibles fuentes de *bugs* decidimos que no podremos empezar la implementación de una fase **sin haber previamente implementado y testeado la anterior**.

## 4. Estructuras nuevas

`struct screen *all_screens`: una lista de todas las pantallas creadas por el sistema operativo. Cuando el usuario haga `'CTRL + TAB'`, accederemos a esta lista para obtener la siguiente pantalla a mostrar.

`struct screen`: representa una pantalla en concreto. Tendrá los siguientes atributos:

* `int screen_id`: id de la pantalla
* `int pid`: pid del proceso que ha creado la pantalla
* `int channel`: canal en el que la pantalla escribe
* `byte x`: posición x del próximo carácter a escribir
* `byte y`: posición y del próximo carácter a escribir

`struct screen *screen_focus`: apunta a la pantalla que tiene el foco, es decir, la que se muestra.

`enum pressed_key`: estará en `interrupt.h` y es un enum que indica si hay una tecla especial pulsada:

* `'NONE'`
* `'CTRL'`
* `'TAB'`

## 5. Estructuras existentes a modificar

`task_struct`: Se añadirán los siguientes atributos:

* `int screen_count`: contador para saber cuántas pantallas ha creado el proceso, y así crear 10 como máximo.
* `struct screen *process_screens`: una lista de las pantallas creadas por el proceso.

## 6. Modificaciones en el código de ZeOS

### `sys_call_table.s`

Añadir las entradas para las llamadas a sistema `createScreen()` y `setFocus(int canal)`, para que el handler de las llamadas a sistema sepa a qué función llamar.

### `user-utils.S`

Añadir los wrappers para `createScreen`, `setFocus` y `removeScreen`, que se llamarán desde código usuario y ejecutarán la llamada a sistema.

### `sys.c`

* Añadir la rutina `sys_createScreen()`, que se encargará de crear una pantalla, es decir, obtener un canal para que el proceso pueda escribir en él.
* Añadir la rutina `sys_setFocus(int canal)`, que cambiará el foto actual, es decir, modificará la estructura screenFocus para que apunte al canal pasado por parámetro.
* Añadir la rutina `switchScreen()`, que será llamada después de un `'CTRL + TAB'`, y llamará a `setFocus()` con el canal obtenido con la estructura `all_screens`.
* Añadir la rutina `sys_removeScreen(int canal)` que se encargará 
Además, se tendrá que modificar la llamada al sistema `fork` para asegurar que:

* Las pantallas se heredan del proceso padre al proceso hijo.
* Se implementa la mejora de eficiencia *Copy-on-Write* propuesta en el enunciado.

### `io.c`

En la rutina `printc`, acceder al `task_struct` del proceso que se está ejecutando mediante la función `current()`, obtener la pantalla en la cual está escribiendo el proceso en ese momento y escribir en ella, con el canal y la posición `(x, y)` indicadas por la pantalla.

### `interrupt.c`

En `keyboard_routine()` hay que comprobar si la tecla pulsada es una tecla especial, y si es así y ha habido una combinación de teclas, realizar la acción correspondiente. Si sólo hay una tecla especial pulsada, actualizar `pressed_key`.

## 7. Juegos de prueba

Cada juego de prueba consistirá en una función de signatura `int test_id(void)` que devolverá `0` si la funcionalidad a testear ha funcionado o `-ERR` si no lo ha hecho, indicando el código de error generado. Estas funciones estarán declaradas e implementadas en `user.c`. Además, alguna de ellas requerirá no solamente que devuelva un valor de `0` sino también una comprobación manual hecha por uno de nosotros.  

* `test_1`
  * **objetivo**: probar la creación de una pantalla para un proceso.
  * **descripción**: se llama a `createScreen` y se escribe `'Hola!'`.
  * **comprobación**: la llamada devuelve `0`. Además aparece el texto escrito.

* `test_2`
  * **objetivo**: probar la creación de pantallas varias pantallas para un proceso.
  * **descripción**: se llama a `createScreen` dos veces. Se escribe `'Hola!'` en el segundo canal y se llama a `setFocus` para mostrar la segunda pantalla.
  * **comprobación**: las llamada devuelven `0`. Además aparece el texto escrito.

* `test_3`
  * **objetivo**: control de errores para canales inexistentes en un proceso.
  * **descripción**: se llama a `createScreen` una vez. Se escribe `'Hola!'` en **otro canal**.
  * **comprobación**: las llamada `write` devuelve un error y nada aparece escrito.

* `test_4`
  * **objetivo**: probar la correcta detección de la combinación de teclas `'CTRL+TAB'`.
  * **descripción**: se llama a `createScreen`dos veces. Se escribe `'Hola!'` en el segundo canal y se pausa la ejecución a la espera de que el usuario introduzca `'CTRL+TAB'`.
  * **comprobación**: las llamada devuelven `0`. Además aparece el texto escrito.

* `test_5`
  * **objetivo**: probar la destrucción de pantallas existentes para un proceso.
  * **descripción**: se llama a `createScreen` una vez. Se escribe `'Hola!'`, se duerme el proceso un tiempo razonable y se llama a `removeScreen`.
  * **comprobación**: las llamada devuelven `0`. Inicialmente aparece `'Hola!'`, pero éste desaparece despues de la destrucción de la pantalla.

* `test_6`
  * **objetivo**: control de errores al destruir canales inexistentes en un proceso.
  * **descripción**: se llama a `createScreen` una vez y se llama a `removeScreen` especificando **otro** canal.
  * **comprobación**: la llamada `removeScreen` devuelve un error.

* `test_7`
  * **objetivo**: herencia de pantallas entre procesos
  * **descripción**: se llama a `createScreen` una vez, se escribe `'Hola, '` y se llama a `fork`. El hijo escribe `'mundo!'`.
  * **comprobación**: las llamada devuelven `0`. Además aparece el texto escrito.

* `test_8`
  * **objetivo**: control de errores al destruir pantallas heredadas.
  * **descripción**: se llama a `createScreen` una vez, se escribe `'Hola!'` y se llama a `fork`. El hijo llama a `removeScreen`.
  * **comprobación**: Aparece el texto escrito y la llamada `removeScreen` devuelve un error.

* `test_9`
  * **objetivo**: multiples pantallas entre procesos
  * **descripción**: se llama a `createScreen` una vez, se escribe `'Hola!'` y se llama a `fork` **dos veces**. Cada hijo llama a `createScreen` y escribe `'Aloh'` en su canal. Se introduce `'CTRL+TAB'` tres veces para comprobar cada pantalla.  
  * **comprobación**: Se visualizan las tres pantallas, cada una con su correspondiente mensaje.

<!--
  1. Creación de pantallas para un único proceso
  2. Cambios de pantalla para un único proceso
  3. Destrucción de pantallas para un único proceso
  4. Herencia de pantallas entre procesos.
  5. Mejora general de rendimiento *Copy-on-Write*
  -->

## 9. Lista de tareas

A continuación se detalla el esquema de desarrollo a seguir, clarificando cada ítem y relacionándolo con su correspondiente set de pruebas descrito en la sección anterior.

### Nivel 1

1. Modificar las estructuras de datos requeridas
    1. Modificar `task_struct` para añadir los atributos `screen_count` y  `*process_screens`.
2. Crear la estructura `screen`, y la enumeración `pressed_key`.
    1. Añadir la lista `*all_screens*`
    2. Añadir el atibuto `screen`

TODO: 

### Nivel 2

TODO: 

3. Destrucción de pantallas para un único proceso
4. Herencia de pantallas entre procesos.

### Nivel 3
TODO:

5. Mejora general de rendimiento *Copy-on-Write*
