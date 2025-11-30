/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Sistemas Operativos
 * 
 * @file backup_server.cc
 * @author Sergio Molina Ríos (alu0101718194@ull.edu.es)
 * @date 2025-11-29
 * @brief implementación de las funciones del backup-server
 */

#include "common_functions.h"
#include <atomic>
#include <../Actividad1.C++/actividad.h>

std::atomic<bool> quit_requested{false};

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


/**
 * @brief Crea una FIFO con la ruta indicada y permisos 0666. Si ya existe, la elimina y la crea
 * 
 * @param fifo_path 
 * @return std::expected<void, std::system_error> 
 */
std::expected<void, std::system_error> create_fifo(const std::string& fifo_path) {

  // Crear la FIFO
  int result = mkfifo(fifo_path.c_str(), 0666);

  // Comprobar si la creación falla, si es así, verificar si es porque ya existe una, y en ese caso, eliminarla y volverla a crear
  if (result == -1) {
    if (errno == EEXIST) {
      if (unlink(fifo_path.c_str())) {
        // Comprobar error al eliminar la FIFO existente
        return std::unexpected(std::system_error(errno, std::system_category(), "Error al eliminar la FIFO ya existente."));
      }

      if (mkfifo(fifo_path.c_str(), 0666) == -1) {
        return std::unexpected(std::system_error(errno, std::system_category(), "Error al crear la FIFO"));
      }

    } else {
      // Retornar el fallo si el error es otro distinto a EEXIST
      return std::unexpected(std::system_error(errno, std::system_category(), "Error al crear la FIFO"));

    }
  }

  return {};
}

/**
 * @brief Escribe el PID del proceso actual en el archivo que se le indica. Creándolo con permisos 0664
 * 
 * @param pid_file_path 
 * @return std::expected<void, std::system_error> 
 */
std::expected<void, std::system_error> write_pid_file(const std::string& pid_file_path) {
  int fd = open(pid_file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);

  if (fd == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al abrir el archivo donde se escribe el PID"));
  }

  int process_pid = getpid(); 

  char* buffer[32];

  std::string pid = std::to_string(process_pid) + '\n';

  int bytes_written = write(fd, pid.c_str(), sizeof(pid));

  if (bytes_written == -1) {
    close(fd);
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al escribir el PID en el archivo"));
  }

  close(fd);
  return {};
}

/**
 * @brief Manejador de señales
 * 
 * @param signum 
 */
void signal_handler(int signum) {

  char* message = "Recibida señal SIGUSR1\n";
  write(STDOUT_FILENO, message, sizeof(message));

  quit_requested = true;
}

/**
 * @brief Función que configura el manejo de señales
 *  1. Crea un conjunto vacío
 *  2. Añade la señal SIGUSR1
 *  3. Bloquea las señales del conjunto
 *  4. Configura el manejo asíncrono de terminación
 * 
 * @return std::expected<void, std::system_error> 
 */
std::expected<void, std::system_error> setup_signal_handler() {
  sigset_t sigset;
  // Crear un conjunto vacío 
  if (sigemptyset(&sigset) == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al iniciar el manejador de señales"));
  }
  
  // Añadir SIGUSR1 al conjunto
  if (sigaddset(&sigset, SIGUSR1) == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al añadir SIGUSR1 al conjunto de señales"));
  }

  // Bloquear SIGUSR1 
  if (sigprocmask(SIG_BLOCK, &sigset, NULL) == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al bloquear las señales"));
  }

  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al instalar el manejador de SIGTERM"));
  }

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al instalar el manejador de SIGINT"));
  }

  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al instalar el manejador de SIGHUP"));
  }

  if (sigaction(SIGQUIT, &sa, NULL) == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al instalar el manejador de SIGQUIT"));  
  }


}

/**
 * @brief Función que lee una ruta
 * 
 * @param fifo_fd 
 * @return std::expected<std::string, std::system_error> 
 */
std::expected<std::string, std::system_error> read_path_from_fifo(int fifo_fd) {

  char buffer[1];
  std::string path_from_fifo;
  while(path_from_fifo.size() < PATH_MAX) {
    ssize_t bytes_read = read(fifo_fd, buffer, 1);

    // Comprobar si ha habido un error al leer
    if (bytes_read == -1) {
      return std::unexpected(std::system_error(errno, std::system_category(), "Error al leer de la FIFO"));
    }

    if (buffer[0] == '\n') {
      // Fin de línea, se sale del bucle
      break;
    }

    // Ir construyendo el path
    path_from_fifo += buffer[0];
  }

  if (path_from_fifo.empty()) {
    std::unexpected(std::system_error(errno, std::system_category(), "Error, la ruta está vacía"));
  }

  return path_from_fifo;
}

/**
 * @brief Implementa el bucle principal del servidor
 * 
 * @param fifo_fd 
 * @param backup_dir 
 */
void run_server(int fifo_fd, const std::string& backup_dir) {
  
  // Primero se perpara el conjunto de señales
  sigset_t signal_set;
  siginfo_t signal_info;

  sigemptyset(&signal_set);
  sigaddset(&signal_set, SIGUSR1);

  // Bucle principal, cuando llegue una señal, quit_requested se pondrá a true, y se acabará el bucle
  while(!quit_requested) {
    
    // Llamar a sigwaitinfo para esperar a la señal que se incluya en el signal_set
    int result = sigwaitinfo(&signal_set, &signal_info);

    // Manejar un posible error al llamar a sigwaitinfo
    if (result == -1) {
      std::cerr << "Error en sigwaitinfo" << std::endl;
      break;
    }

    // Ahora, leer la ruta desde FIFO
    auto path = read_path_from_fifo(fifo_fd);

    if (!path.has_value()) {
      std::cerr << "Error: " << path.error().what() << std::endl;
      continue; // Seguir esperando más señales
    }

    std::string path_result = path.value();

    std::string filename = get_filename(path_result);
    std::string final_rout = backup_dir + "/" + filename;

    // Ahora, se llama a copy
    auto copy = copy_file(path_result, final_rout, 0666);

    // Comprobar un posible error en copy

    if (!copy.has_value()) {
      std::cerr << "backup-server: error al hacer el backup de " << path_result << ": " << copy.error().what() << std::endl;
    } else {
      std::cout << "backup-server: backup completado: " << path_result << " --> " << final_rout << std::endl;
    }
  }
  
}


