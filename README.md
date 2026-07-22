# PS - Programa ADMIN

Proyecto final del curso **Programación de Sistemas** (UNSA 2026).

Herramienta de administración para Linux que integra monitoreo de procesos,
gestión de archivos, respaldo automático, ejecución de comandos y análisis
de scripts Bash.

## Módulos

| Módulo | Encargado | Estado |
|---|---|---|
| Administrador de Tareas (procesos) | Persona 1 | Pendiente |
| Comandos Linux | Persona 1 | Pendiente |
| Shell de Archivos | Persona 2 | Pendiente |
| Respaldos | Persona 2 | Pendiente |
| Análisis de Scripts Bash | Persona 3 | En progreso |
| Cola de Descargas | Persona 3 | En progreso |

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

## Flujo de trabajo en Git

1. Crear una rama por módulo/función asignada (ej. `feature/bash-analyzer`)
2. Programar y probar localmente
3. Commits descriptivos y detallados
4. Pull Request hacia `main` para revisión antes de integrar
