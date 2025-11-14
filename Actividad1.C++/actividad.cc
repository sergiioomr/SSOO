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
#include <iostream>
#include <libgen.h>
#include <expected>

/**
 * @brief That function check if the parameters passed to the function are correct
 *        Also checks if there are the same file
 *
 * @param argc
 * @param argv
 * @return true
 * @return false
 */
bool check_args(int argc, char *argv[])
{

  // Check if there are exactly three arguments. The program name, and two file names
  if (argc != 3)
  {
    std::cerr << "copy: " << argv[0] << " <origen> <destino>" << std::endl;
    return false;
  }

  const char *origen = argv[1];
  const char *destino = argv[2];

  // Check if the origen file exists
  struct stat st_origen, st_destino;
  if (stat(origen, &st_origen) == -1)
  {
    std::cerr << "Error al obtener los atributos del archivo <origen>" << std::endl;
    return false;
  }

  // Return false if the two files are the same
  if ((st_origen.st_ino == st_destino.st_ino) && (st_origen.st_dev == st_destino.st_dev))
  {
    std::cerr << "Los archivos pasados por parámetro son iguales" << std::endl;
    return false;
  }

  return true;
}

/**
 * @brief This function checks if a path is a directory or a files
 *
 * @param path
 * @return true
 * @return false
 */
bool is_directory(const std::string &path)
{
  struct stat st;
  const char *filepath = path.c_str();
  if (stat(filepath, &st) == -1)
  {

    std::cerr << "Error al obtener los atributos del archivo " << path << std::endl;
    return false;
  }

  return S_ISDIR(st.st_mode);
}

/**
 * @brief This function get the filename from the absolute direction path
 *
 * @param path
 * @return std::string
 */
std::string get_filename(const std::string &path)
{
  // Create a buffer
  char buffer[path.size() + 1];

  // Copy to the buffer the strings characters
  path.copy(buffer, path.size(), 0);

  buffer[path.size()] = '\0';

  // Call basename with the buffer
  char *base = basename(buffer);

  return std::string(base);
}

std::expected<void, std::system_error> copy_file(const std::string &src_path, const std::string &dest_path, mode_t dst_perms = 0)
{
}

int main(int argc, char *argv[])
{
}