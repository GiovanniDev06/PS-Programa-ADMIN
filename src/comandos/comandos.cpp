#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <array>
#include <cstdlib>
#include "comandos.h"

// Módulo: Comandos Linux
// Ejecuta comandos del sistema capturando stdout y stderr por separado,
// y mantiene un historial de lo ejecutado.

namespace {

std::string g_ultima_salida;
std::string g_ultimo_error;
std::vector<std::string> g_historial;

// Ejecuta un comando con popen() y devuelve todo lo que escribió en el
// descriptor indicado (1=stdout normal, 2=stderr redirigido a stdout).
std::string ejecutar_y_capturar(const std::string& comando_shell) {
    std::array<char, 256> buffer;
    std::string resultado;

    FILE* pipe = popen(comando_shell.c_str(), "r");
    if (!pipe) {
        return "[comandos] Error: no se pudo abrir el pipe para ejecutar el comando.";
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        resultado += buffer.data();
    }
    pclose(pipe);
    return resultado;
}

} // namespace

void comandos_ejecutar(const char* comando) {
    if (comando == nullptr) return;
    std::string cmd(comando);

    // Salida estándar: se ejecuta el comando redirigiendo su stderr a /dev/null
    // para no mezclarlo con la salida normal.
    g_ultima_salida = ejecutar_y_capturar(cmd + " 2>/dev/null");

    // Errores: se vuelve a ejecutar redirigiendo stdout a /dev/null y
    // capturando solo stderr (2>&1 tras descartar el 1 original).
    g_ultimo_error = ejecutar_y_capturar(cmd + " 2>&1 1>/dev/null");

    g_historial.push_back(cmd);

    std::cout << "[comandos] Comando ejecutado: " << cmd << std::endl;
    if (!g_ultima_salida.empty()) {
        std::cout << "--- Salida ---\n" << g_ultima_salida;
    }
    if (!g_ultimo_error.empty()) {
        std::cout << "--- Errores ---\n" << g_ultimo_error;
    }
}

void comandos_mostrar_salida() {
    if (g_ultima_salida.empty()) {
        std::cout << "[comandos] No hay salida registrada todavia. Ejecuta un comando primero." << std::endl;
        return;
    }
    std::cout << "--- Ultima salida ---\n" << g_ultima_salida;
}

void comandos_mostrar_errores() {
    if (g_ultimo_error.empty()) {
        std::cout << "[comandos] No hay errores registrados en la ultima ejecucion." << std::endl;
        return;
    }
    std::cout << "--- Ultimos errores ---\n" << g_ultimo_error;
}

void comandos_mostrar_historial() {
    if (g_historial.empty()) {
        std::cout << "[comandos] El historial esta vacio." << std::endl;
        return;
    }
    std::cout << "--- Historial de comandos ---" << std::endl;
    for (size_t i = 0; i < g_historial.size(); ++i) {
        std::cout << (i + 1) << ". " << g_historial[i] << std::endl;
    }
}