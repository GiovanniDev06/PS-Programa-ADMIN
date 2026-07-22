#ifndef PROCESOS_H
#define PROCESOS_H

// Módulo: Administrador de Tareas (Persona 1)
// Convención de nombres: procesos_<accion>

void procesos_listar();
void procesos_buscar(const char* nombre);
void procesos_uso_cpu_memoria(int pid);
void procesos_finalizar(int pid);
void procesos_suspender(int pid);
void procesos_reanudar(int pid);
void procesos_arbol();

#endif
