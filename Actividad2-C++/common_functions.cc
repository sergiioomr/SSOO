/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Sistemas Operativos
 * 
 * @file common_functions.cc
 * @author Sergio Molina Ríos (alu0101718194@ull.edu.es)
 * @date 2025-11-29
 * @brief Implementación de las funciones de common_functions.cc
 */
#include "common_functions.h"


std::string get_work_dir_path() {
  std::string var = "BACKUP_WORK_DIR";
  char* value = getenv(var.c_str());
  if (value) {
    return std::string(value);
  } else {
    return std::string();
  }
}

std::string get_fifo_path() {
  std::string work_dir = get_work_dir_path();

  if (work_dir.empty()) {
    return std::string();
  } 

  return work_dir + "\backup.fifo";
}

std::string get_pid_file_path() {
  std::string work_dir = get_work_dir_path();

  if (work_dir.empty()) {
    return std::string();
  }

  return work_dir + "/backup-server.pid";
}

std::string get_client_pid_file_path() {
  std::string work_dir = get_work_dir_path();

  if (work_dir.empty()) {
    return std::string();
  }

  return work_dir + "/backup.pid";
}

std::expected<std::string, std::system_error> get_absolute_path(const std::string& path) {
  // Usar nullptr como 2 argumento reservar memoria automáticamente
  char* resolved_path = realpath(path.c_str(), nullptr);

  if(resolved_path == nullptr) {
    return std::unexpected(std::system_error(errno, std::system_category(), "realpath failed for: " + path));
  }

  std::string result(resolved_path);
  
  // Liberar la memoria reservada antes por realpath
  free(resolved_path);

  return result;
}

bool file_exists(const std::string& path) {
  return (access(path.c_str(), F_OK) == 0);
}

bool is_regular_file(const std::string& path) {
  struct stat st;
  if (stat(path.c_str(), &st) == -1) {
    return false;
  }

  return S_ISREG(st.st_mode);
}

bool is_directory(const std::string& path) {
  struct stat st;
  if (stat(path.c_str(), &st) == -1) {  
    return false;
  } 

  return S_ISDIR(st.st_mode);
}

std::string get_current_dir() {
  char buffer[PATH_MAX];
  if (getcwd(buffer, sizeof(buffer) ) == NULL) {
    std::cerr << "Error al obtener el directorio de trabajo actual" << std::endl; 
    return "";
  }

  return std::string(buffer);
}

std::string get_filename(const std::string& path) {
  // Crear un buffer 
  char buffer[path.size() + 1];

  // Copiar al buffer los caracteres del string
  path.copy(buffer, path.size(), 0);

  buffer[path.size()] = '\0';
  
  // Llamar a basename con el string que copiamos antes en el buffer
  char* base = basename(buffer);

  return std::string(base);
}

/**
 * @brief Función para leer el pid del servidor
 * 
 * @param pid_file_path 
 * @return std::expected<pid_t, std::system_error> 
 */
std::expected<pid_t, std::system_error> read_server_pid(const std::string& pid_file_path) {
  int fd = open(pid_file_path.c_str(), O_RDONLY);

  if (fd < 0) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al abrir el archivo para leer el PID del servidor"));
  }

  char buffer[32];
  ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

  if (bytes_read == -1) {
    close(fd);
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al leer el archivo que contiene el PID del servidor"));
  }

  close(fd);

  if(bytes_read == 0) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error, el archivo está vacío"));
  }


  buffer[bytes_read] = '\0';
  std::string pid_str(buffer);
  pid_t server_pid = std::stoi(pid_str);

  return server_pid;
}

std::expected<pid_t, std::system_error> read_client_pid(const std::string& client_pid_file_path) {
  int fd = open(client_pid_file_path.c_str(), O_RDONLY);

  if (fd < 0) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al abrir el archivo para leer el PID del cliente"));
  }

  char buffer[32];
  ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

  if (bytes_read == -1) {
    close(fd);
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al leer el archivo que contiene el PID del cliente"));
  }

  if (bytes_read == 0) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error, el archivo está vacío o no se ha leído el PID completamente"));
  }

  buffer[bytes_read] = '\0';
  std::string pid_str(buffer);
  pid_t client_pid = std::stoi(pid_str);

  return client_pid;
}

/**
 * @brief Función para comprobar si el proceso con el PID indicado se está ejecutando
 * 
 * @param pid 
 * @return true 
 * @return false 
 */
bool is_server_running(pid_t pid) {
  if(kill(pid, 0)) {
    return true;
  }

  return false;
}



