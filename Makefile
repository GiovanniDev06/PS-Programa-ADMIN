# Makefile - Proyecto ADMIN (Programacion de Sistemas UNSA 2026)

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -pthread
OBJDIR = obj
BINDIR = bin
TARGET = $(BINDIR)/admin

# Cada modulo aporta su .cpp -> se compila a un .o en obj/
SRCS = src/main.cpp \
       src/procesos/procesos.cpp \
       src/archivos/archivos.cpp \
       src/comandos/comandos.cpp \
       src/respaldos/respaldos.cpp \
       src/bash/bash_analyzer.cpp \
       src/descargas/descargas.cpp

OBJS = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Regla por defecto: compilar y enlazar todo
all: $(TARGET)

$(TARGET): $(OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	@echo "Listo: ejecuta con ./$(TARGET)"

# Regla generica: cada .cpp en src/ genera su .o en obj/
$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Targets individuales por modulo, para compilar/probar sin el resto
procesos: $(OBJDIR)/procesos/procesos.o
archivos: $(OBJDIR)/archivos/archivos.o
comandos: $(OBJDIR)/comandos/comandos.o
respaldos: $(OBJDIR)/respaldos/respaldos.o
bash: $(OBJDIR)/bash/bash_analyzer.o
descargas: $(OBJDIR)/descargas/descargas.o

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean run procesos archivos comandos respaldos bash descargas