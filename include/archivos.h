#ifndef ARCHIVOS_H
#define ARCHIVOS_H

// Módulo: Shell de Archivos (Persona 2)
// Convención de nombres: archivos_<accion>

void archivos_listar(const char* ruta);
void archivos_copiar(const char* origen, const char* destino);
void archivos_mover(const char* origen, const char* destino);
void archivos_eliminar(const char* ruta);
void archivos_buscar(const char* patron);
void archivos_estadisticas(const char* ruta);

#endif
