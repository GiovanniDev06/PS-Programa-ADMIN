# Manual de Usuario — PS-Programa ADMIN

**Entregable de:** Mejia Rondan, Giovanni Patrick; Cornejo Hurtado, Dario Rafael; Chavez Cuno, Deivick Paul Eddi
**Proyecto:** Programación de Sistemas, UNSA 2026

Este manual explica, desde el punto de vista de quien va a **usar** el
programa (no de quien lo programó), cómo instalar, compilar y manejar cada
una de las opciones del menú de ADMIN. Si buscas el detalle técnico de la
implementación (qué llamadas al sistema usa cada módulo), consulta
`docs/documentacion_archivos_respaldos.md` y las secciones técnicas del
`README.md`.

---

## 1. Requisitos

- Un entorno Linux (o WSL en Windows).
- Compilador `g++` con soporte para C++17.
- `make`.

No se necesita instalar ninguna librería adicional: el programa solo usa la
biblioteca estándar de C++ y las APIs del sistema (`/proc`, señales, etc.),
que ya vienen con cualquier distribución Linux.

## 2. Instalación y compilación

1. Clona o descarga el repositorio y entra a la carpeta del proyecto:
   ```bash
   git clone https://github.com/GiovanniDev06/PS-Programa-ADMIN.git
   cd PS-Programa-ADMIN
   ```
2. Compila el proyecto completo:
   ```bash
   make
   ```
   Esto genera el ejecutable en `bin/admin`. Si ves errores de compilación,
   revisa que tu `g++` soporte `-std=c++17` (`g++ --version`).
3. Para compilar y ejecutar en un solo paso:
   ```bash
   make run
   ```
4. Para borrar los binarios generados y compilar desde cero:
   ```bash
   make clean
   make
   ```

## 3. Ejecutar el programa

```bash
./bin/admin
```

Al iniciar, verás el menú principal:

```
===== ADMIN - Herramienta de Administracion Linux =====
1. Administrador de Tareas (procesos)
2. Shell de Archivos
3. Comandos Linux
4. Respaldos
5. Analizar script Bash
6. Cola de Descargas
0. Salir
Opcion:
```

Escribe el número de la opción deseada y presiona Enter. Cualquier entrada
que no sea un número válido del menú te pedirá que lo intentes de nuevo, sin
cerrar el programa. La opción `0` siempre regresa al menú anterior (o cierra
el programa, si estás en el menú principal).

---

## 4. Opción 1 — Administrador de Tareas

Muestra y controla los procesos que están corriendo en el sistema, leyendo
la información en tiempo real desde `/proc`.

```
--- Administrador de Tareas ---
1. Listar procesos
2. Buscar proceso por nombre
3. Ver uso de CPU y memoria (por PID)
4. Finalizar proceso (PID)
5. Suspender proceso (PID)
6. Reanudar proceso (PID)
7. Ver arbol de procesos
0. Volver al menu principal
```

- **1. Listar procesos:** imprime una tabla con todos los procesos activos:
  PID, nombre, estado (Ejecutando, Durmiendo, Zombie, etc.) y memoria
  usada en KB.
- **2. Buscar proceso por nombre:** te pide un texto y muestra solo los
  procesos cuyo nombre contiene ese texto (no hace falta el nombre exacto).
- **3. Ver uso de CPU y memoria:** te pide un PID y muestra su estado,
  memoria residente y el tiempo de CPU acumulado en segundos.
- **4. Finalizar proceso:** te pide un PID y le envía la señal para
  terminarlo (equivalente a `kill <pid>`). El programa te avisa si la señal
  se envió correctamente o si falló (por ejemplo, porque el proceso no
  existe o no tienes permisos sobre él).
- **5. Suspender proceso:** pausa la ejecución de un proceso sin matarlo
  (equivalente a `kill -STOP <pid>`). Útil para congelar temporalmente un
  programa que está consumiendo demasiados recursos.
- **6. Reanudar proceso:** continúa un proceso que había sido suspendido
  con la opción anterior (equivalente a `kill -CONT <pid>`).
- **7. Ver árbol de procesos:** muestra la jerarquía completa de procesos
  (quién es el padre de quién), empezando por el proceso raíz del sistema.

> **Tip:** para probar "Finalizar", "Suspender" y "Reanudar" de forma segura,
> abre otra terminal, corre algo inofensivo como `sleep 300`, y usa el PID
> que te muestra `ps` o la opción "Buscar" de este mismo programa (busca
> `sleep`).

## 5. Opción 2 — Shell de Archivos

Permite listar, copiar, mover, eliminar, buscar y ver estadísticas de
archivos y carpetas, sin salir del programa. Consulta
`docs/documentacion_archivos_respaldos.md` para el detalle técnico de este
módulo.

## 6. Opción 3 — Comandos Linux

Ejecuta comandos de la terminal desde dentro del programa, mostrando su
salida y sus errores por separado, y guardando un historial de la sesión.

```
--- Comandos Linux ---
1. Ejecutar comando
2. Mostrar ultima salida
3. Mostrar ultimos errores
4. Mostrar historial
0. Volver al menu principal
```

- **1. Ejecutar comando:** te pide el comando completo (puede tener
  espacios y argumentos, por ejemplo `ls -la /tmp`) y lo corre. Debajo
  muestra automáticamente lo que produjo: su salida normal si tuvo, o sus
  errores si falló.
- **2. Mostrar última salida:** vuelve a imprimir la salida normal del
  último comando ejecutado.
- **3. Mostrar últimos errores:** vuelve a imprimir los errores del último
  comando ejecutado.
- **4. Mostrar historial:** lista, en orden, todos los comandos que
  ejecutaste durante la sesión actual.

> **Nota:** el historial y la última salida/error solo se guardan mientras
> el programa está abierto; se reinician cada vez que lo vuelves a ejecutar.

## 7. Opción 4 — Respaldos

Crea copias de seguridad incrementales de una carpeta y permite restaurar
versiones anteriores. Consulta `docs/documentacion_archivos_respaldos.md`
para el detalle técnico de este módulo.

## 8. Opción 5 — Analizar script Bash

Lee un script `.sh` (sin ejecutarlo) y reporta qué variables y qué ciclos
(`for`, `while`, `until`) detecta en el código, indicando la línea exacta de
cada uno. Los scripts de ejemplo `ejemplos/ejemplo.sh` y
`ejemplos/ejemplo_avanzado.sh` sirven para ver el analizador en acción — el
resultado esperado para ambos está documentado en el `README.md`.

## 9. Opción 6 — Cola de Descargas

Simula una cola de descargas concurrentes (no descarga archivos reales de
internet): puedes encolar descargas, procesarlas (lo que lanza el avance de
cada una en paralelo), consultar el progreso en cualquier momento y revisar
el historial de eventos.

```
--- Cola de Descargas ---
1. Encolar nueva descarga
2. Procesar cola (lanza hilos)
3. Ver progreso
4. Ver historial de eventos
0. Volver al menu principal
```

Un flujo típico es: encolar una o más descargas (opción 1), procesarlas
(opción 2), y luego consultar "Ver progreso" (opción 3) varias veces
seguidas para ver cómo va subiendo el porcentaje.

## 10. Salir del programa

Desde el menú principal, escribe `0` y presiona Enter. El programa muestra
`Saliendo...` y termina.

## 11. Preguntas frecuentes

**¿Por qué "Finalizar" o "Suspender" me dicen que falló?**
Casi siempre es porque el PID no existe (ya terminó) o porque el proceso
pertenece a otro usuario y no tienes permisos para enviarle señales. Prueba
con un proceso propio (por ejemplo, uno que hayas creado tú mismo con
`sleep`).

**¿El módulo de Comandos Linux puede ejecutar cualquier comando?**
Sí, ejecuta cualquier comando de shell disponible en tu sistema. Úsalo con
cuidado: el programa no filtra ni restringe comandos peligrosos.

**¿La Cola de Descargas descarga archivos reales?**
No. Es una simulación pensada para demostrar el manejo de hilos
(`std::thread`), progreso atómico y eventos con mutex, tal como pide el
enunciado del proyecto.
