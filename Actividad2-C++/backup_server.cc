/* funcionamiento básico
  1. Validar variable entorno definida
  2. Verificar que el directorio de destiono existe y es accesible
  3. Comprobar que no haya otro servidor ejecutándose
    3.1 Leer PID contenido en el archivo
    3.2 Usar kill con 0 para verificar si el proceso existe
    3.3 Si existe, mostrar error y terminar
    3.4 Si no existe, mostrar advertencia y continuar
  4. Crear tubería con nombre (FIFO)
    Utilizar mkfifo(), si existe, usar unlink() y crear de nuevo (permisos 0666)
  5. Escribir PID en backup-server.pid
    Escrbir su PID en backup-server.pid. Crear archivo con permisos 0644 y contener el PID como cadena de texto seguida de salto de línea 
  6. Configurar manejo de señales (SIGUSR1)
  
  7. Abrir la FIFO para lectura
  8. Moestra mensaje indicando que está listo
  9. Bucle infinito esperando señales
*/

#include "common_functions.h"

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

bool is_server_running(pid_t pid) {
  if(kill(pid, 0)) {
    return true;
  }

  return false;
}


std::expected<void, std::system_error> create_fifo(const std::string& fifo_path) {


}

std::expected<void, std::system_error> write_pid_file(const std::string& pid_file_path) {

}

std::expected<void, std::system_error> setup_signal_handler() {


}

std::expected<std::string, std::system_error> read_path_from_fifo(int fifo_fd) {


}

void run_server(int fifo_fd, const std::string& backup_dir) {


}


