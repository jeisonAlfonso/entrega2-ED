# Nombre del ejecutable
EXEC = programa

# Compilador
CXX = g++

# Opciones de compilación
CXXFLAGS = -Wall -Wextra -std=c++11

# Archivos fuente
SRCS = main.cpp sistema.cpp
OBJS = main.o sistema.o

# Regla principal: Compilar el programa
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Regla para compilar archivos .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar archivos compilados
clean:
	rm -f $(EXEC) $(OBJS)

# Regla para ejecutar el programa
run: $(EXEC)
	./$(EXEC)
