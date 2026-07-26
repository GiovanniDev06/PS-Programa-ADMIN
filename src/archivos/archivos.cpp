#include "../include/archivos.h"
#include <iostream>
#include <dirent.h>     // Para listar directorios en Linux
#include <sys/stat.h>   // Para obtener estadísticas de archivos
#include <pwd.h>        // Para obtener el nombre del propietario
#include <grp.h>        // Para obtener el nombre del grupo
#include <ctime>        // Para formatear la fecha
#include <cstring>
#include <fcntl.h>      // Para opciones de abrir archivos (O_RDONLY, etc.)
#include <unistd.h>     // Para read, write, close
#include <cstdio>       // Para rename y remove
#include <string>

using namespace std;

// ==========================================
// COMMIT 1: Lectura e Información
// ==========================================

void archivos_listar(const char* ruta) {
    cout << "\n--- Listando directorio: " << ruta << " ---\n";
    
    DIR *dir;
    struct dirent *ent;
    
    // opendir abre el directorio usando llamadas al sistema de Linux
    if ((dir = opendir(ruta)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            // Ignoramos "." y ".." para que la salida sea más limpia
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                cout << "  - " << ent->d_name << endl;
            }
        }
        closedir(dir);
    } else {
        perror("Error al abrir el directorio");
    }
}

void archivos_estadisticas(const char* ruta) {
    struct stat info;
    
    // stat() extrae la metadata del archivo en Linux
    if (stat(ruta, &info) != 0) {
        perror("Error al obtener estadísticas del archivo");
        return;
    }

    cout << "\n--- Estadísticas de: " << ruta << " ---\n";
    cout << "  Tamaño: " << info.st_size << " bytes\n";

    // Obtener nombres de usuario y grupo reales en vez de solo los IDs (UID/GID)
    struct passwd *pw = getpwuid(info.st_uid);
    struct group  *gr = getgrgid(info.st_gid);
    cout << "  Propietario: " << (pw ? pw->pw_name : "Desconocido") << "\n";
    cout << "  Grupo: " << (gr ? gr->gr_name : "Desconocido") << "\n";

    // Evaluar permisos básicos de Linux
    cout << "  Permisos: ";
    cout << (S_ISDIR(info.st_mode) ? "d" : "-");
    cout << ((info.st_mode & S_IRUSR) ? "r" : "-");
    cout << ((info.st_mode & S_IWUSR) ? "w" : "-");
    cout << ((info.st_mode & S_IXUSR) ? "x" : "-");
    cout << "\n";

    // Formatear la fecha de última modificación
    char timebuf[80];
    struct tm* timeinfo = localtime(&info.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", timeinfo);
    cout << "  Última modificación: " << timebuf << "\n";
}

// ==========================================
// COMMIT 2: Manipulación de Archivos
// ==========================================

void archivos_copiar(const char* origen, const char* destino) {
    cout << "\n--- Copiando archivo ---\n";
    
    // Abrir origen en modo solo lectura
    int fuente = open(origen, O_RDONLY);
    if (fuente < 0) {
        perror("Error al abrir archivo de origen");
        return;
    }

    // Abrir/crear destino en modo escritura, truncar si existe, con permisos 0666
    int dest = open(destino, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest < 0) {
        perror("Error al crear archivo de destino");
        close(fuente);
        return;
    }

    char buffer[4096];
    ssize_t bytes_leidos;
    
    // Leer del origen y escribir en el destino en bloques
    while ((bytes_leidos = read(fuente, buffer, sizeof(buffer))) > 0) {
        if (write(dest, buffer, bytes_leidos) != bytes_leidos) {
            perror("Error al escribir en el destino");
            break;
        }
    }

    close(fuente);
    close(dest);
    cout << "  [OK] Copia completada de '" << origen << "' a '" << destino << "'\n";
}

void archivos_mover(const char* origen, const char* destino) {
    cout << "\n--- Moviendo archivo ---\n";
    
    // rename() cambia la ruta del archivo en el sistema
    if (rename(origen, destino) == 0) {
        cout << "  [OK] Archivo movido exitosamente a '" << destino << "'\n";
    } else {
        perror("Error al mover el archivo");
    }
}

void archivos_eliminar(const char* ruta) {
    cout << "\n--- Eliminando archivo ---\n";
    
    // remove() elimina el enlace al archivo (equivalente a rm)
    if (remove(ruta) == 0) {
        cout << "  [OK] Archivo eliminado exitosamente: '" << ruta << "'\n";
    } else {
        perror("Error al eliminar el archivo");
    }
}

// ==========================================
// COMMIT 3: Búsqueda Recursiva
// ==========================================

// Función auxiliar para buscar dentro de carpetas
void buscar_recursivo(const string& ruta_actual, const string& patron) {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(ruta_actual.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            string nombre_archivo = ent->d_name;

            // Ignorar las carpetas ocultas del sistema "." y ".."
            if (nombre_archivo == "." || nombre_archivo == "..") {
                continue;
            }

            string ruta_completa = ruta_actual + "/" + nombre_archivo;

            // Si el nombre del archivo contiene el patrón que buscamos, lo mostramos
            if (nombre_archivo.find(patron) != string::npos) {
                cout << "  [ENCONTRADO] " << ruta_completa << "\n";
            }

            // Si es un directorio (carpeta), nos metemos a buscar ahí dentro
            if (ent->d_type == DT_DIR) {
                buscar_recursivo(ruta_completa, patron);
            }
        }
        closedir(dir);
    }
}

void archivos_buscar(const char* patron) {
    cout << "\n--- Buscando '" << patron << "' desde el directorio actual ---\n";
    
    // Iniciar la búsqueda desde el directorio actual "."
    buscar_recursivo(".", patron);
    
    cout << "--- Fin de la búsqueda ---\n";
}