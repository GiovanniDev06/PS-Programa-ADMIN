#include "../include/respaldos.h"
#include <iostream>
#include <cstdlib>      // Para la función system()
#include <string>
#include <sys/stat.h>   // Para crear el directorio
#include <dirent.h>     // Para listar el directorio

using namespace std;

// Carpeta oculta donde guardaremos todos los backups
const string DIR_BACKUP = "./.mis_respaldos";

// Función auxiliar para asegurarnos de que la carpeta de backups exista
void asegurar_directorio() {
    mkdir(DIR_BACKUP.c_str(), 0777); 
}

void respaldos_crear_incremental(const char* ruta) {
    asegurar_directorio();
    cout << "\n--- Creando respaldo incremental de: " << ruta << " ---\n";
    
    // El archivo snapshot.snar guarda el registro de qué archivos ya se respaldaron
    string snapshot = DIR_BACKUP + "/snapshot.snar";
    
    // Generamos un nombre de archivo único usando la fecha y hora de Linux
    string nombre_archivo = DIR_BACKUP + "/backup_$(date +%s).tar.gz";
    
    // Armamos el comando tar con la bandera -g para que sea incremental
    string comando = "tar -czg " + snapshot + " -f " + nombre_archivo + " " + string(ruta) + " 2>/dev/null";
    
    int resultado = system(comando.c_str());
    if (resultado == 0) {
        cout << "  [OK] Respaldo creado exitosamente.\n";
    } else {
        cout << "  [ERROR] Hubo un problema al crear el respaldo.\n";
    }
}

void respaldos_listar_versiones(const char* ruta) {
    asegurar_directorio();
    cout << "\n--- Versiones de respaldo disponibles ---\n";
    
    DIR *dir;
    struct dirent *ent;
    int contador = 1;

    if ((dir = opendir(DIR_BACKUP.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            string archivo = ent->d_name;
            
            // Solo mostramos los archivos comprimidos tar.gz
            if (archivo.find(".tar.gz") != string::npos) {
                cout << "  [" << contador << "] " << archivo << "\n";
                contador++;
            }
        }
        closedir(dir);
        
        if (contador == 1) {
            cout << "  No se encontraron respaldos.\n";
        }
    } else {
        perror("  Error al leer el directorio de respaldos");
    }
}

void respaldos_restaurar_version(const char* ruta, int version) {
    asegurar_directorio();
    cout << "\n--- Restaurando respaldo en: " << ruta << " ---\n";
    
    
    
    // Comando que extrae los archivos usando tar
    string comando = "for f in " + DIR_BACKUP + "/*.tar.gz; do tar -xzf \"$f\" -C / 2>/dev/null; done";
    
    cout << "  [INFO] Descomprimiendo archivos...\n";
    int resultado = system(comando.c_str());
    
    if (resultado == 0) {
        cout << "  [OK] Restauración completada. (Se aplicaron todos los incrementos)\n";
    } else {
        cout << "  [ERROR] Fallo en la restauración de los archivos.\n";
    }
}