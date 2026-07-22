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

int main() {
    int opcion;
    do {
        mostrar_menu();
        std::cin >> opcion;

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
                bash_analizar_script("ejemplo.sh");
                break;
            case 6:
                descargas_mostrar_progreso();
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
