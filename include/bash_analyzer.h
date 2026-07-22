#ifndef BASH_ANALYZER_H
#define BASH_ANALYZER_H

// Módulo: Análisis de Scripts Bash (Persona 3)
// Convención de nombres: bash_<accion>

void bash_analizar_script(const char* ruta_script);
void bash_detectar_variables(const char* ruta_script);
void bash_detectar_ciclos(const char* ruta_script);

#endif
