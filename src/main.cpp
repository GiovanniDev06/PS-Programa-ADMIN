#include <iostream>
#include <limits>
#include "procesos.h"
#include "archivos.h"
#include "comandos.h"
#include "respaldos.h"
#include "bash_analyzer.h"
#include "descargas.h"

void mostrar_menu() {
    std::cout << "\n===== ADMIN - Herramienta de Administracion Linux =====\n";
    std::cout << "1. Administrador de Tareas (procesos)\n";
    std::cout << "2. Shell de Archivos\n";
    std::cout << "3. Comandos Linux\n";
    std::cout << "4. Respaldos\n";
    std::cout << "5. Analizar script Bash\n";
    std::cout << "6. Cola de Descargas\n";
    std::cout << "0. Salir\n";
    std::cout << "Opcion: ";
}

void mostrar_submenu_descargas() {
    std::cout << "\n--- Cola de Descargas ---\n";
    std::cout << "1. Encolar nueva descarga\n";
    std::cout << "2. Procesar cola (lanza hilos)\n";
    std::cout << "3. Ver progreso\n";
    std::cout << "4. Ver historial de eventos\n";
    std::cout << "0. Volver al menu principal\n";
    std::cout << "Opcion: ";
}

void manejar_menu_descargas() {
    int opcion;
    do {
        mostrar_submenu_descargas();
        std::cin >> opcion;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Intenta de nuevo.\n";
            continue;
        }

        switch (opcion) {
            case 1: {
                std::cout << "Nombre/URL de la descarga: ";
                std::string url;
                std::cin >> url;
                descargas_encolar(url.c_str());
                break;
            }
            case 2:
                descargas_procesar_cola();
                break;
            case 3:
                descargas_mostrar_progreso();
                break;
            case 4:
                descargas_mostrar_eventos();
                break;
            case 0:
                break;
            default:
                std::cout << "Opcion invalida.\n";
        }
    } while (opcion != 0);
}

int main() {
    int opcion;
    do {
        mostrar_menu();
        std::cin >> opcion;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Intenta de nuevo.\n";
            continue;
        }

        switch (opcion) {
            case 1:
                procesos_listar();
                break;
            case 2:
                archivos_listar(".");
                break;
            case 3:
                comandos_ejecutar("echo prueba");
                break;
            case 4:
                respaldos_listar_versiones(".");
                break;
            case 5:
                bash_analizar_script("ejemplos/ejemplo.sh");
                break;
            case 6:
                manejar_menu_descargas();
                break;
            case 0:
                std::cout << "Saliendo...\n";
                break;
            default:
                std::cout << "Opcion invalida.\n";
        }
    } while (opcion != 0);

    return 0;
}