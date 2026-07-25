#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include "bash_analyzer.h"

// Patron de variable: admite prefijos opcionales (export, local, readonly,
// declare con flags tipo -i/-x/-a) y asignaciones simples (=) o compuestas (+=)
static const std::regex PATRON_VARIABLE(
    R"(^\s*(?:export|local|readonly|declare(?:\s+-[A-Za-z]+)?)?\s*([A-Za-z_][A-Za-z0-9_]*)\+?=(.*)$)"
);

// Patron de ciclo: for, while y until (los 3 tipos de ciclo que tiene Bash)
static const std::regex PATRON_CICLO(R"(^\s*(for|while|until)\b)");

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

        size_t primer_no_espacio = linea.find_first_not_of(" \t");
        if (primer_no_espacio == std::string::npos) continue;
        if (linea[primer_no_espacio] == '#') continue; // comentario

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

        size_t primer_no_espacio = linea.find_first_not_of(" \t");
        if (primer_no_espacio == std::string::npos) continue;
        if (linea[primer_no_espacio] == '#') continue; // comentario

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