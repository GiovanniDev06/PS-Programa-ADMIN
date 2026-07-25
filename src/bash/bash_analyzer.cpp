#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include "bash_analyzer.h"

// Patron para detectar asignacion de variable estilo Bash: NOMBRE=valor
// (sin espacios alrededor del '=', como exige la sintaxis de Bash)
static const std::regex PATRON_VARIABLE(R"(^\s*([A-Za-z_][A-Za-z0-9_]*)=(.*)$)");

// Patron para detectar inicio de ciclo for/while
static const std::regex PATRON_CICLO(R"(^\s*(for|while)\b)");

void bash_detectar_variables(const char* ruta_script) {
    std::ifstream archivo(ruta_script);
    if (!archivo.is_open()) {
        std::cout << "[bash] Error: no se pudo abrir " << ruta_script << std::endl;
        return;
    }

    std::string linea;
    int numero_linea = 0;
    int total_encontradas = 0;

    std::cout << "\n--- Variables detectadas en " << ruta_script << " ---\n";

    while (std::getline(archivo, linea)) {
        numero_linea++;

        // Ignorar comentarios
        size_t primer_no_espacio = linea.find_first_not_of(" \t");
        if (primer_no_espacio != std::string::npos && linea[primer_no_espacio] == '#') {
            continue;
        }

        std::smatch coincidencia;
        if (std::regex_search(linea, coincidencia, PATRON_VARIABLE)) {
            std::cout << "  Linea " << numero_linea << ": " << coincidencia[1]
                      << " = " << coincidencia[2] << std::endl;
            total_encontradas++;
        }
    }

    std::cout << "Total de variables detectadas: " << total_encontradas << std::endl;
}

void bash_detectar_ciclos(const char* ruta_script) {
    std::ifstream archivo(ruta_script);
    if (!archivo.is_open()) {
        std::cout << "[bash] Error: no se pudo abrir " << ruta_script << std::endl;
        return;
    }

    std::string linea;
    int numero_linea = 0;
    int total_encontrados = 0;

    std::cout << "\n--- Ciclos detectados en " << ruta_script << " ---\n";

    while (std::getline(archivo, linea)) {
        numero_linea++;

        std::smatch coincidencia;
        if (std::regex_search(linea, coincidencia, PATRON_CICLO)) {
            std::cout << "  Linea " << numero_linea << ": tipo '" << coincidencia[1]
                      << "' -> " << linea << std::endl;
            total_encontrados++;
        }
    }

    std::cout << "Total de ciclos detectados: " << total_encontrados << std::endl;
}

void bash_analizar_script(const char* ruta_script) {
    std::cout << "===== Analizando script: " << ruta_script << " =====" << std::endl;
    bash_detectar_variables(ruta_script);
    bash_detectar_ciclos(ruta_script);
}