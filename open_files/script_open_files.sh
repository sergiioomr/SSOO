#!/usr/bin/env bash

PROGNAME=(basename $0)
directory_filter=
# Si la variable de entorno OPEN_FILES_FOLDER no está vacía
if [ -n "$OPEN_FILES_FOLDER" ]; then
  if [ -d "$OPEN_FILES_FOLDER" ]; then
    directory_filter="$OPEN_FILES_FOLDER"
  else
    error_exit "$LINENO: El directorio de la variable OPEN_FILES_FOLDER no existe"
  fi
fi

# Variable para recorrer después todos los usuarios conectados y mostrar lo que se desee de cada uno
# who muestra los conectados, awk ... cogerá todos los elementos de la primera columna de cada línea, y sort -u los ordenará eliminando también duplicados
connected_users=$(who | awk '{print $1}' | sort -u)

default_option() {


for user in $connected_users; do
  # Ahora para cada usuario, se deben de mostrar los procesos con lsof -u "$user"
  # Ahora, estos procesos, se filtran por la columna Type, que es la quinta, si esta es igual a REG, entonces se hace el print
  # Por último, wc -l va contando todas las líneas que le llegan
  if [-n "$directory_filter" ]; then
    number_files=$(lsof -u "$user" -a +D "$directory_filter" | awk '$5 == "REG" {print}' | wc -l)
  else 
    number_files=$(lsof -u "$user" | awk '$5 == "REG" {print}' | wc -l)
  fi

  old_pid=$(ps -u "$user" --sort=start_time | awk 'NR==2 {print$1}')
  user_id=$(id -u "$user")

  echo "$user $user_id $old_pid $number_files"

done 
}

# Función para explicar el funcionamiento del script
usage() 
{
  echo "Usage: open_files [-h] [--help] [-f regexp] [-o] [--offline] [-u user1 user2 ...] [-s] [--sort]"
}

# Función para manejar los errores
error_exit()
{
  echo "${PROGNAME}: ${1:-Error desconocido}" > &2
  exit 1
}

# Procesamiento de parámetros
while [ "$1" != "" ]; do
  case $1 in 
    -h | --help )
      $(usage)
      ;;
    -f )
      
      ;;
    -o | --offline )

      ;;
    -u )

      ;;
    -s | --sort )

      ;;
    * )
      $(usage)
      error_exit "$LINENO: Opción no soportada: $1"
      ;;
  esac
  shift
done

type -P lsof || error_exit "$LINENO: No se ha encontrado el comando lsof"
default_option