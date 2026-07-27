# PS - Programa ADMIN

Proyecto final del curso **Programación de Sistemas** (UNSA 2026).

Herramienta de administración para Linux que integra monitoreo de procesos,
gestión de archivos, respaldo automático, ejecución de comandos y análisis
de scripts Bash.

## Módulos

| Módulo | Encargado |
|---|---|
| Administrador de Tareas (procesos) | Mejia Rondan, Giovanni Patrick |
| Comandos Linux | Mejia Rondan, Giovanni Patrick |
| Shell de Archivos | Chavez Cuno Deivick Paul Eddi |
| Respaldos | Chavez Cuno Deivick Paul Eddi |
| Análisis de Scripts Bash | Cornejo Hurtado Dario Rafael |
| Cola de Descargas | Cornejo Hurtado Dario Rafael |

## Requisitos

- Entorno Linux (o WSL en Windows)
- g++ con soporte C++17
- make

## Compilación

```bash
make          # Compila todo y genera bin/admin
make run      # Compila y ejecuta directamente
make clean    # Borra los binarios generados
```

También se puede compilar un módulo individual mientras se desarrolla, por ejemplo:

```bash
make procesos
make bash
```

## Estructura del repositorio

```
include/    -> headers (.h) de cada módulo
src/        -> implementación (.cpp) de cada módulo, main.cpp integra todo
docs/       -> documentación técnica y manual de usuario
```

## Convención de nombres de funciones

Cada módulo expone sus funciones con un prefijo fijo para evitar choques de nombres:

- `procesos_*` — Administrador de Tareas
- `comandos_*` — Comandos Linux
- `archivos_*` — Shell de Archivos
- `respaldos_*` — Respaldos
- `bash_*` — Análisis de Scripts Bash
- `descargas_*` — Cola de Descargas

## Módulo: Administrador de Tareas

Trabaja directamente sobre el pseudo-sistema de archivos `/proc`, la misma
fuente que usan herramientas estándar como `ps` y `top`, en vez de invocar
esos comandos externos.

**Funciones (`procesos.h` / `procesos.cpp`):**
- `procesos_listar()` — recorre `/proc` (cada subcarpeta numérica es un PID)
  y muestra PID, nombre, estado y memoria residente (`VmRSS`) de cada
  proceso, leyendo `/proc/<pid>/status`.
- `procesos_buscar(nombre)` — igual que listar, pero filtra por coincidencia
  parcial del nombre.
- `procesos_uso_cpu_memoria(pid)` — muestra el detalle de un proceso puntual:
  estado, memoria RSS y tiempo de CPU acumulado (usuario + sistema), este
  último calculado a partir de los campos `utime`/`stime` de
  `/proc/<pid>/stat` divididos entre los ticks de reloj del sistema
  (`sysconf(_SC_CLK_TCK)`).
- `procesos_finalizar(pid)` / `procesos_suspender(pid)` / `procesos_reanudar(pid)`
  — envían `SIGTERM`, `SIGSTOP` y `SIGCONT` respectivamente con `kill()` de
  `<signal.h>`, e informan si la señal se entregó o si falló (por ejemplo,
  por falta de permisos o porque el proceso ya no existe).
- `procesos_arbol()` — arma la relación padre-hijo de todos los procesos
  (usando el `PPid` de `/proc/<pid>/status`) y la imprime en forma de árbol
  recursivo, comenzando en `init` (PID 1) cuando está disponible.

### Submenú del programa (opción 1 del menú principal)

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

### Ejemplo de uso y resultado

Suspendiendo y luego reanudando un proceso (`sleep 60`, PID 738):

```
Opcion: 5
PID: 738
[procesos] suspender (SIGSTOP) enviado correctamente al PID 738

Opcion: 6
PID: 738
[procesos] reanudar (SIGCONT) enviado correctamente al PID 738
```

Verificando con `ps -o pid,stat,cmd -p 738` después de reanudar, el proceso
queda en estado `S` (durmiendo), no `T` (detenido) — confirmando que la
reanudación funcionó correctamente.

## Módulo: Comandos Linux

Ejecuta comandos de shell arbitrarios capturando su salida estándar y su
salida de error **por separado**, y guarda un historial de todo lo
ejecutado en la sesión.

**Funciones (`comandos.h` / `comandos.cpp`):**
- `comandos_ejecutar(comando)` — ejecuta el comando dos veces con `popen()`:
  una vez redirigiendo `stderr` a `/dev/null` (para quedarse solo con la
  salida estándar) y otra redirigiendo `stdout` a `/dev/null` (para
  quedarse solo con los errores). Guarda ambos resultados y agrega el
  comando al historial.
- `comandos_mostrar_salida()` — reimprime la última salida estándar
  capturada.
- `comandos_mostrar_errores()` — reimprime los últimos errores capturados.
- `comandos_mostrar_historial()` — lista, numerados en orden cronológico,
  todos los comandos ejecutados en la sesión.

### Submenú del programa (opción 3 del menú principal)

```
--- Comandos Linux ---
1. Ejecutar comando
2. Mostrar ultima salida
3. Mostrar ultimos errores
4. Mostrar historial
0. Volver al menu principal
```

### Ejemplo de uso y resultado

```
Opcion: 1
Comando a ejecutar: echo hola mundo
[comandos] Comando ejecutado: echo hola mundo
--- Salida ---
hola mundo

Opcion: 1
Comando a ejecutar: comando_que_no_existe_xyz
[comandos] Comando ejecutado: comando_que_no_existe_xyz
--- Errores ---
sh: 1: comando_que_no_existe_xyz: not found

Opcion: 4
--- Historial de comandos ---
1. echo hola mundo
2. comando_que_no_existe_xyz
```

## Módulo: Análisis de Scripts Bash

Lee un script `.sh` línea por línea y detecta patrones de sintaxis Bash,
sin ejecutar el script (análisis léxico, no interpretación).

**Detecta como variables:**
- Asignaciones simples: `nombre=valor`
- Asignaciones compuestas: `contador+=1`
- Declaraciones con prefijo: `export`, `local`, `readonly`, `declare -i/-x/-a`

**Detecta como ciclos:**
- `for` (incluyendo estilo clásico `for i in ...` y estilo C `for (( ; ; ))`)
- `while`
- `until`

**Evita falsos positivos:** no marca comparaciones dentro de condicionales
(`if [ "$a" == "$b" ]`) ni contenido de líneas comentadas (`#`).

**Funciones (`bash_analyzer.h` / `bash_analyzer.cpp`):**
- `bash_analizar_script(ruta)` — corre el análisis completo (variables + ciclos)
- `bash_detectar_variables(ruta)` — solo variables
- `bash_detectar_ciclos(ruta)` — solo ciclos

**Scripts de prueba** (`ejemplos/`):
- `ejemplo.sh` — caso simple, para demo
- `ejemplo_avanzado.sh` — casos límite (prefijos de declaración, `+=`, `until`,
  `for` estilo C, comparaciones en condicionales) para validar que no haya
  falsos positivos ni negativos

### ejemplos/ejemplo.sh

```bash
#!/bin/bash

# Script de ejemplo para probar el analizador

contador=0
nombre="admin"
total=10

for i in 1 2 3 4 5
do
    contador=$((contador + 1))
    echo "Iteracion: $i"
done

while [ $contador -lt $total ]
do
    contador=$((contador + 1))
done

echo "Contador final: $contador"
```

**Resultado del análisis:**

```
--- Variables detectadas en ejemplos/ejemplo.sh ---
  Linea 5: contador = 0
  Linea 6: nombre = "admin"
  Linea 7: total = 10
  Linea 11: contador = $((contador + 1))
  Linea 17: contador = $((contador + 1))
Total de variables detectadas: 5

--- Ciclos detectados en ejemplos/ejemplo.sh ---
  Linea 9: tipo 'for' -> for i in 1 2 3 4 5
  Linea 15: tipo 'while' -> while [ $contador -lt $total ]
Total de ciclos detectados: 2
```

### ejemplos/ejemplo_avanzado.sh

```bash
#!/bin/bash
# Script de prueba avanzado - casos limite

contador=0
nombre="Dario Cornejo"
ruta=/home/usuario/admin
lista=(1 2 3 4 5)

export PATH_ADMIN=/opt/admin
local temporal=5
readonly MAX_INTENTOS=3
declare -i puntaje=100
declare -x MODO=produccion

# Esto es un comentario con variable=falsa, no debe detectarse
    # Comentario indentado, tampoco debe detectarse

contador+=1

if [ "$nombre" == "admin" ]; then
    echo "coincide"
fi

if [[ $contador -eq 0 ]]; then
    echo "cero"
fi

for i in 1 2 3 4 5
do
    contador=$((contador + 1))
done

for (( j=0; j<10; j++ ))
do
    echo "$j"
done

while [ $contador -lt $MAX_INTENTOS ]
do
    contador=$((contador + 1))
done

until [ $contador -ge 20 ]
do
    contador=$((contador + 1))
done

for k in $(ls); do
    echo "$k"
done
```

**Resultado del análisis:**

```
--- Variables detectadas en ejemplos/ejemplo_avanzado.sh ---
  Linea 4: contador = 0
  Linea 5: nombre = "Dario Cornejo"
  Linea 6: ruta = /home/usuario/admin
  Linea 7: lista = (1 2 3 4 5)
  Linea 9: PATH_ADMIN = /opt/admin
  Linea 10: temporal = 5
  Linea 11: MAX_INTENTOS = 3
  Linea 12: puntaje = 100
  Linea 13: MODO = produccion
  Linea 18: contador = 1
  Linea 30: contador = $((contador + 1))
  Linea 40: contador = $((contador + 1))
  Linea 45: contador = $((contador + 1))
Total de variables detectadas: 13

--- Ciclos detectados en ejemplos/ejemplo_avanzado.sh ---
  Linea 28: tipo 'for' -> for i in 1 2 3 4 5
  Linea 33: tipo 'for' -> for (( j=0; j<10; j++ ))
  Linea 38: tipo 'while' -> while [ $contador -lt $MAX_INTENTOS ]
  Linea 43: tipo 'until' -> until [ $contador -ge 20 ]
  Linea 48: tipo 'for' -> for k in $(ls); do
Total de ciclos detectados: 5
```

Nótese que ninguna de las comparaciones dentro de `if [ ]` / `if [[ ]]`, ni el
texto de los comentarios, fueron marcadas como variables — confirmando que el
analizador no genera falsos positivos con estos casos.

## Módulo: Cola de Descargas

**Importante: esto es una simulación, no descarga archivos reales de internet.**
El objetivo es demostrar gestión de una cola concurrente (hilos, memoria
compartida protegida) tal como pide el enunciado ("cola de descargas,
progreso y eventos"), sin necesidad de tráfico de red real.

### Diseño

- Cada descarga es un registro con **id, nombre, progreso (0-100%) y estado**
  (`En cola`, `Descargando`, `Completado`).
- Al procesar la cola, se lanza **un hilo (`std::thread`) por cada descarga
  pendiente**, todos corriendo en paralelo — así se demuestra concurrencia
  real, no una simulación secuencial.
- El progreso de cada descarga se guarda en una variable **atómica**
  (`std::atomic<int>`), para que varios hilos puedan escribir/leer el valor
  sin corromper los datos (evita condiciones de carrera).
- El registro de eventos está protegido con un **mutex**, para que si dos
  hilos terminan casi al mismo tiempo, sus mensajes no se mezclen.
- Los hilos usan `detach()`: el menú del programa sigue respondiendo mientras
  las descargas avanzan en segundo plano.

### Funciones (`descargas.h` / `descargas.cpp`)

- `descargas_encolar(nombre)` — agrega una nueva descarga a la cola, en
  estado "En cola", y registra el evento correspondiente.
- `descargas_procesar_cola()` — toma todas las descargas pendientes de la
  cola y lanza un hilo por cada una, que simula el avance del progreso en
  pasos de 10% cada ~150ms hasta llegar a 100%.
- `descargas_mostrar_progreso()` — imprime el estado actual de todas las
  descargas registradas, con una barra visual `[####------] 40%`.
- `descargas_mostrar_eventos()` — imprime el historial completo de eventos
  (encolada, iniciada, completada) en orden cronológico.

### Submenú del programa (opción 6 del menú principal)

```
--- Cola de Descargas ---
1. Encolar nueva descarga
2. Procesar cola (lanza hilos)
3. Ver progreso
4. Ver historial de eventos
0. Volver al menu principal
```

1. **Encolar nueva descarga**: pide un nombre (ej. `kernel-linux-6.9.tar.gz`)
   y lo agrega a la cola. En este punto la descarga queda en 0% y estado
   "En cola" — todavía no avanza.
2. **Procesar cola**: toma todo lo que esté pendiente y arranca los hilos.
   A partir de aquí el progreso empieza a subir en segundo plano.
3. **Ver progreso**: se puede llamar varias veces seguidas para ver cómo
   sube el porcentaje de cada descarga mientras los hilos corren.
4. **Ver historial de eventos**: muestra el log completo, útil para
   verificar el orden de encolado/inicio/finalización.

### Ejemplo de uso y resultado

Encolando `kernel-linux-6.9.tar.gz` y `backup-sistema.zip`, luego procesando
la cola y consultando el progreso dos veces seguidas:

```
--- Estado de descargas ---
  id 1 | kernel-linux-6.9.tar.gz | [----------] 0% | Descargando
  id 2 | backup-sistema.zip | [----------] 0% | Descargando

--- Estado de descargas ---
  id 1 | kernel-linux-6.9.tar.gz | [########--] 80% | Descargando
  id 2 | backup-sistema.zip | [########--] 80% | Descargando

--- Historial de eventos ---
  1. Encolada: kernel-linux-6.9.tar.gz
  2. Encolada: backup-sistema.zip
  3. Iniciando descarga: kernel-linux-6.9.tar.gz
  4. Iniciando descarga: backup-sistema.zip
```

Nótese que ambas descargas avanzan al mismo ritmo porque corren en hilos
paralelos independientes, no una después de la otra.

## Flujo de trabajo en Git

1. Crear una rama por módulo/función asignada (ej. `feature/bash-analyzer`)
2. Programar y probar localmente
3. Commits descriptivos y detallados
4. Pull Request hacia `main` para revisión antes de integrar
