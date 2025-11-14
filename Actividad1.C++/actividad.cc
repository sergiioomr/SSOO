// 1. Procesamiento de parámetros --> ./actividad origen destino
// 2. Manejo de posibles errores
// 3. Implementación del código
//  3.1 Comprobar validez de los argumentos de línea de comandos
//  3.2 Comprobar si destino es un directorio
//  3.3 Obtener el nombre del archivo desde una ruta
//  3.4 Copiar el archivo
// 4. Comprobaciones
#include <sys/stat.h>
#include <iostream>
#include <libgen.h>
#include <expected>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

bool check_args(int argc, char* argv[]) {
  
  if (argc != 3) {
    std::cerr << "copy: " << argv[0] << " <origen> <destino>" << std::endl;
    return false;
  }

  const char* origen = argv[1];
  const char* destino = argv[2];

  struct stat st_origen, st_destino;
  if (stat(origen, &st_origen) == -1) {
    std::cerr << "Error al obtener los atributos del archivo <origen>" << std::endl;
    return false;
  } 

  if ((st_origen.st_ino == st_destino.st_ino) && (st_origen.st_dev == st_destino.st_dev)) {
    std::cerr << "Los archivos pasados por parámetro son iguales" << std::endl;
    return false;
  }

  return true;
}



bool is_directory(const std::string& path) {
  struct stat st;
  const char* filepath = path.c_str();
  if (stat(filepath, &st) == -1) {
    
    std::cerr << "Error al obtener los atributos del archivo " << path << std::endl;
    return false;

  } 

  return S_ISDIR(st.st_mode);
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

std::expected<void, std::system_error>  copy_file(const std::string& src_path, const std::string& dest_path, mode_t dst_perms=0) {
  
  int fd_src = open(src_path.c_str(), O_RDONLY);
  if (fd_src < 0) {
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al abrir el archivo de origen"));

  }

  int fd_dest = open(dest_path.c_str(), O_WRONLY | O_CREAT, dst_perms);
  if (fd_dest < 0) {
    close(fd_src);
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al abrir el archivo de destino"));
  }

  // Create a buffer to save the information in the file reading
  char buffer [65536];
  ssize_t bytes_read = read(fd_src, buffer, sizeof(buffer));

  while (bytes_read > 0) {

    ssize_t bytes_written = write(fd_dest, buffer, bytes_read);
    if (bytes_written < 0 ) {
      close(fd_src);
      close(fd_dest);
      return std::unexpected(std::system_error(errno, std::system_category(), "Error en la copia del archivo. No se han podido escribir todos los Bytes"));
    }

    bytes_read = read(fd_src, buffer, sizeof(buffer));

  }

  if (bytes_read < 0) {
    close(fd_src);
    close(fd_dest);
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al abrir el archivo de destino"));
  }

  close(fd_src);
  close(fd_dest);

  return {};
}






int main(int argc, char* argv[]) {
  bool result = check_args(argc, argv);
  // If has error_(result) 
  // If is_directory(destino)
  const char* origen = argv[1];
  const char* destino = argv[2];
  copy_file(origen, destino);

}