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

#include "backup-server.h"

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
/*std::expected<void, std::system_error> write_pid_file(const std::string& pid_file_path) {
  int fd = open(pid_file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);

  if (fd == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al abrir el archivo donde se escribe el PID"));
  }

  int process_pid = getpid(); 

  std::string pid = std::to_string(process_pid) + '\n';

  int bytes_written = write(fd, pid.c_str(), pid.length());

  if (bytes_written == -1) {
    close(fd);
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al escribir el PID en el archivo"));
  }

  close(fd);
  return {};
}
*/
std::expected<void, std::system_error> write_pid_file(const std::string& pid_file_path) {
  std::cerr << "DEBUG: Intentando crear archivo PID en: " << pid_file_path << std::endl;
  
  int fd = open(pid_file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);

  if (fd == -1) {
    std::cerr << "DEBUG: Error al abrir, errno: " << errno << std::endl;
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al abrir el archivo donde se escribe el PID"));
  }

  int process_pid = getpid(); 
  std::cerr << "DEBUG: PID a escribir: " << process_pid << std::endl;

  std::string pid = std::to_string(process_pid) + '\n';

  int bytes_written = write(fd, pid.c_str(), pid.length());

  if (bytes_written == -1) {
    std::cerr << "DEBUG: Error al escribir, errno: " << errno << std::endl;
    close(fd);
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al escribir el PID en el archivo"));
  }

  std::cerr << "DEBUG: Bytes escritos: " << bytes_written << std::endl;
  close(fd);
  return {};
}
/**
 * @brief Manejador de señales
 * 
 * @param signum 
 */
void signal_handler(int signum) {

  const char message[] = "backup-server: señal de terminación recibida\n";
  write(STDOUT_FILENO, message, sizeof(message) - 1);
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

  return {};
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

    if (bytes_read == 0) {
      return std::unexpected(std::system_error(errno, std::system_category(), "La FIFO se ha cerrado inesperadamente"));
    }
    if (buffer[0] == '\n') {
      // Fin de línea, se sale del bucle
      break;
    }

    // Ir construyendo el path
    path_from_fifo += buffer[0];
  }

  if (path_from_fifo.empty()) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error, la ruta está vacía"));
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

  std::cout << "backup-server: esperando solicitudes de backup en " << backup_dir << std::endl;

  // Bucle principal, cuando llegue una señal, quit_requested se pondrá a true, y se acabará el bucle
  while(!quit_requested) {
    
    // Llamar a sigwaitinfo para esperar a la señal que se incluya en el signal_set
    int result = sigwaitinfo(&signal_set, &signal_info);

    // Manejar un posible error al llamar a sigwaitinfo
    if (result == -1) {
      if (result == EINTR) {
        continue;
      }
      std::cerr << "Error en sigwaitinfo" << std::endl;
      break;
    }

    if (quit_requested) {
      break;
    }


    // Ahora, leer la ruta desde FIFO
    
    auto path_result = read_path_from_fifo(fifo_fd);

    if (!path_result.has_value()) {
      std::cerr << "backup-server: error: fallo al leer la ruta desde la FIFO" << std::endl;
      continue;
    }

    std::string origen_path = path_result.value();

    if (origen_path.empty()){
      std::cerr << "backup-server: error: La ruta leía está vacía" << std::endl;
      continue;
    }

    std::string filename = get_filename(origen_path);
    std::string destiny_path = backup_dir + "/" + filename;

    auto copy_result = copy_file(origen_path, destiny_path, 0664);

    if (!copy_result.has_value()) {
      std::cerr << "backupk-server: errror al hacer backup" << std::endl;
    } else {
      std::cout << "backup-server: backup completado" << std::endl;
    }
  }

  std::cout << "backup-server: cerrando servidor..." << std::endl;
}

int main(int argc, char* argv[]) {
  // Primero, compruebo si se ha pasado ruta de destino
  std::string backup_dir;
  if (argc > 1) {
    backup_dir = argv[1];
  } else {
    backup_dir = get_current_dir();
  }

  // 1. Validar BACKUP_WORK_DIR
  std::string work_dir = get_work_dir_path();
  if (work_dir.empty()) {
    std::cerr << "backup-server: error: BACKUP_WORK_DIR no está definida" << std::endl;
    return EXIT_FAILURE;
  }

  // 2. Verificar que el directorio de destino existe y es accesible. También que el directorio de trabajo exista
  if (!is_directory(backup_dir)) {
    std::cerr << "backup-server: error: el directorio de destino " << backup_dir << " no existe o no es accesible" << std::endl;
    return EXIT_FAILURE;
  }

  if (!is_directory(work_dir)) {
    std::cerr << "backup-server: error: el directorio de trabajo " << work_dir << " no existe" << std::endl;
  }

  // 3. Comprobar que no haya otro servidor ejecutándose
  std::string pid_file = get_pid_file_path();
  if (file_exists(pid_file)) {
    auto pid = read_server_pid(pid_file);
    if (pid && is_server_running(pid.value())) {
      std::cerr << "backup-server: error: ya hay un servidor ejecutándose" << std::endl;
      return EXIT_FAILURE;
    } else {
      std::cerr << "backup-server: error: archivo de un servidor anterior" << std::endl;
    } 
  }

  // 4. Crear la FIFO
  std::string fifo_path = get_fifo_path();
  auto result_fifo_create = create_fifo(fifo_path);

  if (!result_fifo_create.has_value()) {
    std::cerr << "backup-server: error: " << result_fifo_create.error().what() << std::endl;
    return EXIT_FAILURE;
  }

  // 5. Escribir el PID en el archivo backup-server.pid
  auto result_write_pid = write_pid_file(pid_file);
  if (!result_write_pid.has_value()) {
    std::cerr << "backup-server: error: " << result_write_pid.error().what() << std::endl;
    unlink(fifo_path.c_str());
    return EXIT_FAILURE; 
  }
  
  // 6. Configurar el manejo de señales
  auto result_signal = setup_signal_handler();
  if (!result_signal.has_value()) {
    std::cerr << "backup-server: error: " << result_signal.error().what() << std::endl;
    unlink(fifo_path.c_str());
    unlink(pid_file.c_str());
    return EXIT_FAILURE;
  }

  // 7. Abrir la FIFO para lectura
  int fd = open(fifo_path.c_str(), O_RDONLY);
  if (fd == -1) {
    std::cerr << "backup-server: error: Error al abrir la FIFO" << std::endl;
    unlink(fifo_path.c_str());
    unlink(pid_file.c_str());
    return EXIT_FAILURE;
  }

  // 8. Ejecutar el servidor
  run_server(fd, backup_dir);

  // 9. Limpiar recursos usados
  close(fd);
  unlink(fifo_path.c_str());
  unlink(pid_file.c_str());

  std::cout << "backup-server: servidor terminado" << std::endl;
  
  return EXIT_SUCCESS;
}




