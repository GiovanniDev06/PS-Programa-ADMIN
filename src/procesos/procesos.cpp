#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <cctype>
#include "procesos.h"

// Módulo: Administrador de Tareas
// Implementado leyendo directamente el pseudo-sistema de archivos /proc,
// que es la fuente estándar de información de procesos en Linux (misma
// fuente que usan herramientas como ps/top).

namespace {

struct InfoProceso {
    int pid = 0;
    int ppid = 0;
    std::string nombre;
    char estado = '?';
    long vm_rss_kb = 0; // memoria residente en KB
};

// Devuelve true si la cadena representa un PID (solo dígitos).
bool es_pid(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

// Lee /proc/<pid>/status y arma la info básica del proceso.
bool leer_info_proceso(int pid, InfoProceso& info) {
    std::ifstream status("/proc/" + std::to_string(pid) + "/status");
    if (!status.is_open()) return false; // el proceso pudo haber terminado

    info.pid = pid;
    std::string linea;
    while (std::getline(status, linea)) {
        if (linea.rfind("Name:", 0) == 0) {
            std::istringstream iss(linea.substr(5));
            iss >> info.nombre;
        } else if (linea.rfind("State:", 0) == 0) {
            std::istringstream iss(linea.substr(6));
            std::string letra;
            iss >> letra;
            if (!letra.empty()) info.estado = letra[0];
        } else if (linea.rfind("PPid:", 0) == 0) {
            info.ppid = std::stoi(linea.substr(5));
        } else if (linea.rfind("VmRSS:", 0) == 0) {
            std::istringstream iss(linea.substr(6));
            iss >> info.vm_rss_kb;
        }
    }
    return true;
}

// Obtiene el uso de CPU acumulado (en segundos) leyendo /proc/<pid>/stat.
bool leer_cpu_segundos(int pid, double& segundos) {
    std::ifstream stat("/proc/" + std::to_string(pid) + "/stat");
    if (!stat.is_open()) return false;

    std::string contenido;
    std::getline(stat, contenido);

    size_t cierre = contenido.rfind(')');
    if (cierre == std::string::npos) return false;

    std::istringstream resto(contenido.substr(cierre + 2));
    std::string campo;
    std::vector<std::string> campos;
    while (resto >> campo) campos.push_back(campo);

    if (campos.size() < 13) return false;
    long utime = std::stol(campos[11]);
    long stime = std::stol(campos[12]);
    long hz = sysconf(_SC_CLK_TCK);
    if (hz <= 0) hz = 100;
    segundos = static_cast<double>(utime + stime) / hz;
    return true;
}

std::vector<int> listar_pids() {
    std::vector<int> pids;
    DIR* dir = opendir("/proc");
    if (!dir) return pids;

    struct dirent* entrada;
    while ((entrada = readdir(dir)) != nullptr) {
        std::string nombre = entrada->d_name;
        if (es_pid(nombre)) pids.push_back(std::stoi(nombre));
    }
    closedir(dir);
    return pids;
}

std::string nombre_estado(char c) {
    switch (c) {
        case 'R': return "Ejecutando";
        case 'S': return "Durmiendo";
        case 'D': return "Espera-IO";
        case 'Z': return "Zombie";
        case 'T': return "Detenido";
        case 't': return "Trace-stop";
        default:  return "Desconocido";
    }
}

void imprimir_encabezado() {
    std::cout << std::left
               << std::setw(8)  << "PID"
               << std::setw(20) << "NOMBRE"
               << std::setw(12) << "ESTADO"
               << std::setw(12) << "MEM(KB)"
               << std::endl;
    std::cout << std::string(52, '-') << std::endl;
}

void imprimir_fila(const InfoProceso& info) {
    std::cout << std::left
               << std::setw(8)  << info.pid
               << std::setw(20) << info.nombre
               << std::setw(12) << nombre_estado(info.estado)
               << std::setw(12) << info.vm_rss_kb
               << std::endl;
}

void enviar_senal(int pid, int senal, const char* accion) {
    if (kill(pid, senal) == 0) {
        std::cout << "[procesos] " << accion << " enviado correctamente al PID " << pid << std::endl;
    } else {
        std::cout << "[procesos] Error al intentar " << accion
                   << " el PID " << pid << " (verifica que exista y que tengas permisos)."
                   << std::endl;
    }
}

} // namespace

void procesos_listar() {
    std::vector<int> pids = listar_pids();
    imprimir_encabezado();
    for (int pid : pids) {
        InfoProceso info;
        if (leer_info_proceso(pid, info)) {
            imprimir_fila(info);
        }
    }
    std::cout << "Total de procesos: " << pids.size() << std::endl;
}

void procesos_buscar(const char* nombre) {
    if (nombre == nullptr) return;
    std::string objetivo(nombre);
    std::vector<int> pids = listar_pids();
    bool encontrado = false;

    imprimir_encabezado();
    for (int pid : pids) {
        InfoProceso info;
        if (leer_info_proceso(pid, info) && info.nombre.find(objetivo) != std::string::npos) {
            imprimir_fila(info);
            encontrado = true;
        }
    }
    if (!encontrado) {
        std::cout << "No se encontraron procesos que coincidan con \"" << objetivo << "\"." << std::endl;
    }
}

void procesos_uso_cpu_memoria(int pid) {
    InfoProceso info;
    if (!leer_info_proceso(pid, info)) {
        std::cout << "[procesos] No se pudo leer informacion del PID " << pid
                   << " (¿existe?)." << std::endl;
        return;
    }

    double cpu_segundos = 0.0;
    leer_cpu_segundos(pid, cpu_segundos);

    std::cout << "PID:            " << info.pid << std::endl;
    std::cout << "Nombre:         " << info.nombre << std::endl;
    std::cout << "Estado:         " << nombre_estado(info.estado) << std::endl;
    std::cout << "Memoria (RSS):  " << info.vm_rss_kb << " KB" << std::endl;
    std::cout << "CPU acumulada:  " << std::fixed << std::setprecision(2) << cpu_segundos << " s" << std::endl;
}

void procesos_finalizar(int pid) {
    enviar_senal(pid, SIGTERM, "finalizar (SIGTERM)");
}

void procesos_suspender(int pid) {
    enviar_senal(pid, SIGSTOP, "suspender (SIGSTOP)");
}

void procesos_reanudar(int pid) {
    enviar_senal(pid, SIGCONT, "reanudar (SIGCONT)");
}

void procesos_arbol() {
    std::vector<int> pids = listar_pids();

    std::map<int, std::vector<int>> hijos;
    std::map<int, InfoProceso> info_por_pid;

    for (int pid : pids) {
        InfoProceso info;
        if (leer_info_proceso(pid, info)) {
            info_por_pid[pid] = info;
            hijos[info.ppid].push_back(pid);
        }
    }

    std::function<void(int, int)> imprimir_rama = [&](int pid, int nivel) {
        auto it = info_por_pid.find(pid);
        if (it == info_por_pid.end()) return;

        std::cout << std::string(nivel * 2, ' ')
                   << (nivel > 0 ? "|- " : "")
                   << it->second.nombre << " (PID " << pid << ")" << std::endl;

        for (int hijo : hijos[pid]) {
            if (hijo != pid) imprimir_rama(hijo, nivel + 1);
        }
    };

    if (info_por_pid.count(1)) {
        imprimir_rama(1, 0);
    } else {
        for (const auto& par : info_por_pid) {
            if (!info_por_pid.count(par.second.ppid)) {
                imprimir_rama(par.first, 0);
            }
        }
    }
}