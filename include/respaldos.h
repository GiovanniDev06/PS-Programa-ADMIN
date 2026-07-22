#ifndef RESPALDOS_H
#define RESPALDOS_H

// Módulo: Respaldos (Persona 2)
// Convención de nombres: respaldos_<accion>

void respaldos_crear_incremental(const char* ruta);
void respaldos_restaurar_version(const char* ruta, int version);
void respaldos_listar_versiones(const char* ruta);

#endif
