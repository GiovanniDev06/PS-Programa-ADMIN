#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <string>
#include <sstream>
#include "descargas.h"

// Representa una descarga individual dentro del sistema
struct Descarga {
    int id;
    std::string nombre;
    std::atomic<int> progreso{0};      // 0-100, se actualiza desde el hilo que la procesa
    std::string estado = "En cola";    // En cola / Descargando / Completado
};

// Estado global del modulo de descargas
static std::vector<std::shared_ptr<Descarga>> g_todas;      // historial completo (para mostrar progreso)
static std::queue<std::shared_ptr<Descarga>> g_pendientes;  // cola FIFO de lo que falta procesar
static std::vector<std::string> g_eventos;                  // log de eventos
static std::mutex g_mtx_estado;                             // protege g_todas y g_pendientes
static std::mutex g_mtx_eventos;                            // protege g_eventos
static int g_siguiente_id = 1;

static void registrar_evento(const std::string& mensaje) {
    std::lock_guard<std::mutex> lock(g_mtx_eventos);
    g_eventos.push_back(mensaje);
}

// Funcion que corre en un hilo aparte: simula la descarga de un archivo
static void procesar_una_descarga(std::shared_ptr<Descarga> d) {
    d->estado = "Descargando";
    registrar_evento("Iniciando descarga: " + d->nombre);

    for (int avance = 0; avance <= 100; avance += 10) {
        d->progreso = avance;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    d->estado = "Completado";
    registrar_evento("Descarga completada: " + d->nombre);
}

void descargas_encolar(const char* url) {
    auto nueva = std::make_shared<Descarga>();
    nueva->id = g_siguiente_id++;
    nueva->nombre = url;

    {
        std::lock_guard<std::mutex> lock(g_mtx_estado);
        g_todas.push_back(nueva);
        g_pendientes.push(nueva);
    }

    registrar_evento("Encolada: " + nueva->nombre);
    std::cout << "[descargas] Encolado (id " << nueva->id << "): " << url << std::endl;
}

void descargas_procesar_cola() {
    std::vector<std::shared_ptr<Descarga>> pendientes_a_procesar;

    {
        std::lock_guard<std::mutex> lock(g_mtx_estado);
        while (!g_pendientes.empty()) {
            pendientes_a_procesar.push_back(g_pendientes.front());
            g_pendientes.pop();
        }
    }

    if (pendientes_a_procesar.empty()) {
        std::cout << "[descargas] No hay descargas pendientes en la cola." << std::endl;
        return;
    }

    std::cout << "[descargas] Procesando " << pendientes_a_procesar.size()
              << " descarga(s) en paralelo..." << std::endl;

    // Lanza un hilo por cada descarga pendiente: se procesan de forma concurrente.
    // Se usa detach() porque el programa (menu) debe seguir respondiendo mientras
    // las descargas avanzan en segundo plano; el progreso se puede consultar en
    // cualquier momento porque esta protegido con std::atomic.
    for (auto& d : pendientes_a_procesar) {
        std::thread hilo(procesar_una_descarga, d);
        hilo.detach();
    }
}

void descargas_mostrar_progreso() {
    std::lock_guard<std::mutex> lock(g_mtx_estado);

    if (g_todas.empty()) {
        std::cout << "[descargas] No hay descargas registradas." << std::endl;
        return;
    }

    std::cout << "\n--- Estado de descargas ---" << std::endl;
    for (auto& d : g_todas) {
        int p = d->progreso.load();
        int bloques_llenos = p / 10;
        std::string barra = "[";
        for (int i = 0; i < 10; i++) barra += (i < bloques_llenos) ? "#" : "-";
        barra += "]";

        std::cout << "  id " << d->id << " | " << d->nombre << " | "
                  << barra << " " << p << "% | " << d->estado << std::endl;
    }
}

void descargas_mostrar_eventos() {
    std::lock_guard<std::mutex> lock(g_mtx_eventos);

    if (g_eventos.empty()) {
        std::cout << "[descargas] No hay eventos registrados." << std::endl;
        return;
    }

    std::cout << "\n--- Historial de eventos ---" << std::endl;
    for (size_t i = 0; i < g_eventos.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << g_eventos[i] << std::endl;
    }
}