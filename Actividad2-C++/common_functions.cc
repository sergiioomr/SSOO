// Archivo que contiene funciones comunes útiles para ambos programas
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

  return work_dir + "\backup-server.pid";
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
    exit;
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




