/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Sistemas Operativos
 * 
 * @file backup.cc
 * @author Sergio Molina Ríos (alu0101718194@ull.edu.es)
 * @date 2025-11-29
 * @brief Archivo donde se definen las funciones del programa backup1
 */

#include "common_functions.h"
#include "backup-server.h"
#include <atomic> 

std::atomic<bool> all_ok{false};


bool check_args(int argc, char* argv[]) {
  
  if (argc != 2) {
    std::cerr << "backup-server: " << argv[0] << " <destino>" << std::endl;
    return false;
  }

  return true;
}

std::expected<void, std::system_error> check_work_dir_exists(const std::string& work_dir) {
  if (!is_directory(work_dir))  {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error, el directorio de destino no existe o no es un directorio"));
  }

  return {};
}

std::expected<void, std::system_error> write_client_pid_file(const std::string& backup_pid_file_path) {
  int fd = open(backup_pid_file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);

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

std::expected<int, std::system_error> open_fifo_write(const std::string& fifo_path) {
  int fd = open(fifo_path.c_str(), O_WRONLY);
  if (fd == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error, no se ha podido abrir la FIFO"));
  }

  return fd;
}


std::expected<void, std::system_error> write_path_to_fifo(int fifo_fd, const std::string& file_path) {
  std::string final_path = file_path + "\n";

  int bytes_written = write(fifo_fd, final_path.c_str(), final_path.length());

  if (bytes_written == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al escribrir en la FIFO"));
  }

  int length = final_path.length();
  if (bytes_written != length) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Escritura en la FIFO incompleta"));
  }
  
  return {};
}

std::expected<void, std::system_error> write_pid_to_fifo(int fifo_fd, pid_t pid) {
  std::string pid_string = std::to_string(pid);

  int bytes_written = write(fifo_fd, pid_string.c_str(), pid_string.length());

  if (bytes_written == -1) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al escribir el pid en la FIFO"));
  }

  int length = pid_string.length();
  if (bytes_written != length) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Escritura del PID en la FIFO inconmpleta"));
  }
  
  return {};
}


int main(int argc, char* argv[]) {
  // 1. Validar argumentos 
  check_args(argc, argv);

  // El cliente ahora debe esperar alguna de las señales SIGUSR1 O SIGUSR2 y ejecutar lo que proceda en cada caso
  sigset_t sigset;
  siginfo_t siginfo;

  sigemptyset(&sigset);
  sigaddset(&sigset, SIGUSR1);
  sigaddset(&sigset, SIGUSR2);


  int result = sigwaitinfo(&sigset, &siginfo);

  if (result == -1) {
    std::cerr << "error en sigwaitinfo" << std::endl;
    return EXIT_FAILURE;
  }


  // 2. Validar BACKUP_WORK_DIR
  std::string work_dir = get_work_dir_path();
  if (work_dir.empty()) {
    std::cerr << "backup: error: BACKUP_WORK_DIR no está definida" << std::endl;
    return EXIT_FAILURE;
  }

  // 3. Verificar que el archivo existe y es un archivo regular. También que el directorio de trabajo existe
  auto work_dir_exist = check_work_dir_exists(work_dir);
  if (!work_dir_exist.has_value()) {
    std::cerr << "backup: error: " << work_dir_exist.error().what() << std::endl;
  }

  std::string file_path = argv[1];
  if (!file_exists(file_path)) {
    std::cerr << "backup: error: el archivo no existe" << std::endl;
  }

  if (!is_regular_file(file_path)) {
    std::cerr << "backup: error: el archivo no es un archivo regular" << std::endl;
  }

  // 4. Leer el PID del servidor
  std::string pid_file = get_pid_file_path();
  if (!file_exists(pid_file)) {
    std::cerr << "backup: error: el servidor no está ejecutándose (el archivo PID no existe)" << std::endl;
    return EXIT_FAILURE;
  }

  auto pid_result = read_server_pid(pid_file);
  if (!pid_result.has_value()) {
    std::cerr << "backup: error: " << pid_result.error().what() << std::endl;
    return EXIT_FAILURE;
  }

  pid_t server_pid = pid_result.value();

  // 5. Verificar que el servidor está ejecutándose
  if (is_server_running(server_pid)) {
    std::cerr << "backup: error: El servidor no se está ejecutando" << std::endl;
    return EXIT_FAILURE;
  }

  // 6. Bloquea la señal SIGPIPE
  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGPIPE);

  if (sigprocmask(SIG_BLOCK, &sigset, NULL) == -1) {
    std::cerr << "backup: error al bloquear el SIGPIPE" << std::endl;
    return EXIT_FAILURE;
  }

  // 7. Abrir la FIFO para la escritura
  std::string fifo_path = get_fifo_path();
  int fd = open(fifo_path.c_str(), O_WRONLY);

  if (fd == -1) {
    std::cerr << "backup: error: No se ha podido abrir la FIFO" << std::endl;
    return EXIT_FAILURE;
  }

  // 8. Convierte la ruta del archivo a ruta absoluta
  auto absolute_path_result = get_absolute_path(file_path);
  if (!absolute_path_result.has_value()) {
    std::cerr << "backup : error: " << absolute_path_result.error().what() << std::endl;
    close(fd);
    return EXIT_FAILURE;
  }

  std::string absolute_path = absolute_path_result.value();

  // 9. Escribir la ruta en la FIFO y el pid del proceso
  auto write_path_result = write_path_to_fifo(fd, absolute_path);
  if (!write_path_result.has_value()) {
    std::cerr << "backup: error: " << write_path_result.error().what() << std::endl;
    close(fd);
    return EXIT_FAILURE;
  }

  pid_t pid_client = getpid();
  auto write_pid_result = write_pid_to_fifo(fd, pid_client);
  if (!write_pid_result.has_value()) {
    std::cerr << "backup: error: " << write_pid_result.error().what() << std::endl;
    close(fd);
    return EXIT_FAILURE;
  }

  // 10. Enviar la señal SIGUSR1 al servidor
  if (kill(server_pid, SIGUSR1)) {
    std::cerr << "backup: error: no se ha podido enviar la señal SIGUSR1" << std::endl;
    close(fd);
    return EXIT_FAILURE;
  }

  // 11. Cerrar la FIFO
  close(fd);

  // 12. Mensaje de confirmación
  std::cout << "solicitud enviada" << std::endl;

  sleep(1000);


  return EXIT_SUCCESS;
}