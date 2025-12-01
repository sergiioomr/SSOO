CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra 

COMMON_OBJ = common_functions.o
COPY_OBJ = copy.o

all: clean backup-server backup

# --- Objetos comunes ---
common_functions.o: common_functions.cc common_functions.h
	$(CXX) $(CXXFLAGS) -c common_functions.cc

copy.o: copy.cc copy.h common_functions.h
	$(CXX) $(CXXFLAGS) -c copy.cc

# --- Ejecutable: backup-server ---
backup-server: backup-server.o $(COMMON_OBJ) $(COPY_OBJ)
	$(CXX) $(CXXFLAGS) backup-server.o $(COMMON_OBJ) $(COPY_OBJ) -o backup-server

backup-server.o: backup-server.cc backup-server.h copy.h common_functions.h
	$(CXX) $(CXXFLAGS) -c backup-server.cc

# --- Ejecutable: backup ---
backup: backup.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) backup.o $(COMMON_OBJ) -o backup

backup.o: backup.cc common_functions.h 
	$(CXX) $(CXXFLAGS) -c backup.cc

# --- Limpieza ---
clean:
	rm -f *.o backup-server backup

rebuild: clean all

.PHONY: all clean rebuild
