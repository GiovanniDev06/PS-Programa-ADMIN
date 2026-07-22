#include <iostream>
#include "descargas.h"

void descargas_encolar(const char* url) {
    std::cout << "[descargas] Encolando: " << url << std::endl;
    // TODO: agregar a una estructura de cola (std::queue)
}

void descargas_mostrar_progreso() {
    std::cout << "[descargas] Mostrando progreso..." << std::endl;
    // TODO: mostrar % de avance de la descarga activa
}

void descargas_mostrar_eventos() {
    std::cout << "[descargas] Mostrando eventos..." << std::endl;
    // TODO: log de eventos (inicio, error, completado)
}

void descargas_procesar_cola() {
    std::cout << "[descargas] Procesando cola..." << std::endl;
    // TODO: usar un hilo (std::thread) para procesar sin bloquear el menú
}
