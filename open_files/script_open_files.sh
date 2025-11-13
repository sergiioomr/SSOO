#!/usr/bin/env bash

PROGNAME=(basename $0)
regexpresion=
directory_filter=
users_optionu_list=
list_users=
u_option=
sort_option=

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


default_option() {

  if [ "$u_option" != 1 ]; then
    list_users=$(echo $users_optionu_list | tr ' ' '\n' | sort -u) # Se hace esto para ordenarla y evitar repetidos
  else
    list_users=$(who | awk '{print $1}' | sort -u)
  fi

for user in $list_users; do
  # Ahora para cada usuario, se deben de mostrar los procesos con lsof -u "$user"
  # Ahora, estos procesos, se filtran por la columna Type, que es la quinta, si esta es igual a REG, entonces se hace el print
  # Por último, wc -l va contando todas las líneas que le llegan
  if [ -n "$directory_filter" ]; then
    number_files=$(lsof -u "$user" -a +D "$directory_filter" | awk '$5 == "REG" {print}' | wc -l)
  else 
    number_files=$(lsof -u "$user" | awk '$5 == "REG" {print}' | wc -l)
  fi

  old_pid=$(ps -u "$user" --sort=start_time | awk 'NR==2 {print$1}')
  user_id=$(id -u "$user")

  echo "$user $user_id $old_pid $number_files"

done 
}

filter_option() {
  # Cuando se llame a esta función significa que se habrá usado el parámetro -f
  # Modificar la expresión regular para añadirle el símbolo $ al final.
  regexpresion=$regexpresion$
  for user in $connected_users; do
    if [ -n "$directory_filter" ]; then
      number_files=$(lsof -u "$user" -a +D "$directory_filter" | awk '$5 == "REG" {print}' | grep -E "$prueba" | wc -l)
    else 
      number_files=$(lsof -u "$user" | awk '$5 == "REG" {print}' | grep -E "$prueba" | wc -l)
    fi

    old_pid=$(ps -u "$user" --sort=start_time | awk 'NR==2 {print$1}')
    user_id=$(id -u $user)

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
      shift
      regexpresion=$1
      ;;
    -o | --offline )

      ;;

    -u )
      u_option=1
      shift
      while [ [ "$1" != -* ] ] && [ -n "$1" ]; do
        users_u_list="$users_u_list $1"
        shift
      done

      ;;

    -s | --sort )
      sort_option=1
      ;;
    * )
      $(usage)
      error_exit "$LINENO: Opción no soportada: $1"
      ;;
  esac
  shift
done

type -P lsof || error_exit "$LINENO: No se ha encontrado el comando lsof"

if [ "$sort_option" = 1 ]; then
  default_option | sort -k4
fi

default_option