# Documentación Técnica: Módulos de Archivos y Respaldos

**Responsable:** Persona 2
**Entorno de ejecución:** Linux (WSL)
**Lenguaje:** C++

---

## 1. Módulo de Archivos (Shell de Archivos)

Este módulo expone una interfaz en C++ que emula el comportamiento de los comandos básicos de manipulación de archivos en sistemas Unix/Linux, interactuando directamente con las llamadas al sistema (System Calls) de la API POSIX.

### Funcionalidades implementadas:
*   **Listado de directorios (`archivos_listar`):** Utiliza la librería `<dirent.h>` (`opendir`, `readdir`, `closedir`) para iterar sobre los nodos de un directorio, omitiendo las referencias relativas del sistema (`.` y `..`).
*   **Estadísticas de metadatos (`archivos_estadisticas`):** Hace uso de la estructura `stat` de `<sys/stat.h>` para extraer metadatos a bajo nivel, incluyendo tamaño en bytes, ID del propietario y grupo (resolviendo los nombres reales con `<pwd.h>` y `<grp.h>`), permisos en formato `rwx` y el *timestamp* de la última modificación.
*   **Transferencia de datos (`archivos_copiar`):** Abre descriptores de archivos con `<fcntl.h>` (`open`) y utiliza un buffer de memoria de 4096 bytes para leer (`read`) y escribir (`write`) datos binarios o de texto de manera eficiente entre el origen y el destino.
*   **Manipulación de rutas (`archivos_mover` y `archivos_eliminar`):** Emplea las funciones estándar de `<cstdio>` (`rename` y `remove`) para alterar la tabla de asignación de archivos del sistema sin tener que reescribir los datos en el disco.
*   **Búsqueda recursiva (`archivos_buscar`):** Implementa un algoritmo de búsqueda en profundidad (DFS) que recorre las carpetas verificando el tipo de nodo (`DT_DIR`). Si encuentra coincidencias con el patrón de texto proporcionado, concatena e imprime la ruta absoluta.

---

## 2. Módulo de Respaldos (Backups Incrementales)

Este módulo se encarga de la seguridad e integridad de los datos, delegando las operaciones de compresión y versionado a las herramientas nativas del entorno Linux mediante la función `system()` de `<cstdlib>`.

### Arquitectura de los Backups:
*   **Directorio oculto:** Todos los respaldos y registros se almacenan de forma segura en un directorio local llamado `./.mis_respaldos`. Si no existe, el sistema lo crea dinámicamente con permisos `0777` utilizando `mkdir()`.
*   **Lógica Incremental (`respaldos_crear_incremental`):** Aprovecha la potencia del binario `tar` de Linux con la bandera `--listed-incremental` (`-g`). Esto genera un archivo de control llamado `snapshot.snar` que registra los metadatos de los archivos. En ejecuciones posteriores, `tar` compara los archivos actuales contra el *snapshot* y comprime (en formato `.tar.gz`) **únicamente** los archivos que han sido modificados o creados desde el último respaldo.
*   **Restauración (`respaldos_restaurar_version`):** Utiliza un bucle de *shell script* inyectado desde C++ para descomprimir secuencialmente los archivos `.tar.gz`. Al extraer todos los incrementos sobre el mismo directorio base, el sistema de archivos recupera automáticamente la versión más reciente de cada documento modificado.

---

## 3. Guía de Ejecución

Gracias a la integración del `Makefile` en la raíz del repositorio, probar estos módulos es directo:

1. Levantar el entorno WSL en la terminal.
2. Compilar y ejecutar todo el proyecto con un solo comando:
   ```bash
   make run