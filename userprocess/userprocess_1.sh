# #!/usr/bin/env bash

# Lista para almacenar los usuarios pasados por parámetro
users_list=
min_time=0
users=
sort_order=2
# Opción sin parámetros

show_process() {
  if [ "$users" = 1 ]; then
    for user in $users_list; do
    # si la opción users es uno, es que se debe usar la lista de usuarios, así que aquí se comprueba que el usuario sea igual al que toca en la iteración simplemente
      ps axu | awk -v u="$user" '$1 == u {print$1} ' | uniq -c
    done
  else 
    # Sino, saldrá la opción por defecto, si se ha activado el tiempo, se comprobará con este, sino se comprobará con 0 que es lo mismo que nada
    ps axu | tail -n+2 | awk -v t="$min_time" '$10 > t {print$1}' | sort | uniq -c | sort -k2 # Añado el filtro del tiempo, si este no se indicara, sería 0, por lo que sería lo mismo que no hacerlo
  fi
}

usage() {
  echo "Usage: userprocess [-u] [user1 user2 user3...] [-m] [cant. tiempo] [-n]"
}

error_exit() {
  echo "${PROGNAME}: ${1:-Error desconocido}" >&2
  exit 1
}

# Procesamiento de parámetros
while [ "$1" != "" ]; do
  case $1 in
    -u )
      users=1
      shift
      while [[ "$1" != -* ]] && [ "$1" != "" ]; do
        users_list="$users_list $1"
        shift
      done
    ;;
    -m )
      shift
      if [ "$1" = "" ]; then
        error_exit "Opción -m requiere un valor temporal"
      fi
      min_time=$1
    ;;
    -n )
      sort_order=1
      ;;
    * )
      usage
      error_exit "$LINENO: Opción no soportada: $1"
      ;;
  esac
  shift
done

show_process | sort -r -k"$sort_order"
