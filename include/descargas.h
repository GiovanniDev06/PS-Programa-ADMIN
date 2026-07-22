#ifndef DESCARGAS_H
#define DESCARGAS_H

// Módulo: Cola de Descargas (Persona 3)
// Convención de nombres: descargas_<accion>

void descargas_encolar(const char* url);
void descargas_mostrar_progreso();
void descargas_mostrar_eventos();
void descargas_procesar_cola();

#endif
