#include <iostream>
#include "bash_analyzer.h"

void bash_analizar_script(const char* ruta_script) {
    std::cout << "[bash] Analizando script: " << ruta_script << std::endl;
    // TODO: abrir el archivo, leer línea por línea y clasificar contenido
}

void bash_detectar_variables(const char* ruta_script) {
    std::cout << "[bash] Detectando variables en: " << ruta_script << std::endl;
    // TODO: buscar patrones tipo NOMBRE=valor
}

void bash_detectar_ciclos(const char* ruta_script) {
    std::cout << "[bash] Detectando ciclos en: " << ruta_script << std::endl;
    // TODO: buscar palabras clave "for" y "while"
}
