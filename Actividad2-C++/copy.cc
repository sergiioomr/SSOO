// 1. Procesamiento de parámetros --> ./actividad origen destino
// 2. Manejo de posibles errores
// 3. Implementación del código
//  3.1 Comprobar validez de los argumentos de línea de comandos
//  3.2 Comprobar si destino es un directorio
//  3.3 Obtener el nombre del archivo desde una ruta
//  3.4 Copiar el archivo
// 4. Comprobaciones
#include "copy.h"

[[nodiscard]]
std::expected<void, std::system_error>  copy_file(const std::string& src_path, const std::string& dest_path, mode_t dst_perms) {
  
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
    return std::unexpected(std::system_error(errno, std::system_category(), "Error al leer el archivo origen"));
  }

  close(fd_src);
  close(fd_dest);

  return {};
}
