#!/usr/bin/env bash

# sysinfo - Un script que informa del estado del sistema

#### Opciones por defecto.
interactive=
mostrar=
filename=~/sysinfo.txt
PROGNAME=(basename $0)

#### Constantes

TITLE="Información de mi sistema para $HOSTNAME"
RIGHT_NOW=$(date +"%x %R %Z")
TIME_STAMP="Actualizada en el $RIGHT_NOW por $USER"

#### Estilos

TEXT_BOLD=$'\x1b[1m'
TEXT_GREEN=$'\x1b[32m'
TEXT_RESET=$'\x1b[0m'
TEXT_ULINE=$'\x1b[4m'

#### Funciones

error_exit()
{
# Función para salir en caso de error.
# Acepta un argumento, cadena con el mensaje de error
  echo "${PROGNAME}: ${1:-Error desconocido}" > &2
  exit 1

}
system_info() 
{
    echo "${TEXT_ULINE}Versión del sistema${TEXT_RESET}"
    echo
    uname -a
    echo
}

sys_info()
{
  echo "${TEXT_ULINE}Número de procesos ejecutados por root en este momento:${TEXT_RESET}"
  echo
  ps -u root | wc -l
  echo
}

show_uptime() 
{
    echo "${TEXT_ULINE}Tiempo de encendido del sistema$TEXT_RESET"
    echo
    uptime
    echo
}

drive_space()
{
    echo "${TEXT_ULINE}Espacio ocupado en las particiones/discos duros del sistema${TEXT_RESET}"
    echo
    df | tr -s ' ' | cut -d ' ' -f 3 | tail -n+2 | awk '{sum += $1} END {print sum}'
    echo
}

home_space()
{
    echo "${TEXT_ULINE}Espacio ocupado en los subdirectorios / directorio personal${TEXT_RESET}"
    echo
    printf "%-10s %-12s %-10s %-10s\s" "ARCHIVOS" "DIRECTORIOS" "USADO" "DIRECTORIO"
    
    
}

interactive_mode()
{

  echo -n "Mostrar el informe del sistema en pantalla (S/N): "
  read mostrar
  if [ "$mostrar" = "S" ]; then
    write_page
    exit 0
  fi

  echo -n "Introduzca el nombre del archivo [$filename]: "
  read input_filename

  # Si lo que se lee por consola no está vacío, ese será el archivo nuevo, si es vacío, sigo con el por defecto
  if [ "$input_filename" != "" ]; then
    filename=$input_filename
  fi

  # Para la última parte de la opción interactiva, con la opción -f compruebo si un archivo existe y es regular
  if [ -f "$filename" ]; then 
    echo -n "El archivo de destino existe. ¿Sobreescribir? (S/N): "
    read eleccion
    if [ "$eleccion" != "S" ] && [ "$eleccion" != "s" ]; then
      exit 0
    fi
  fi
}

#### Programa principal
usage()
{
  echo "usage: sysinfo [-f filename] [-i] [-h]"
}

write_page() 
{
  cat << _EOF_
$TEXT_BOLD$TITLE$TEXT_RESET
$(system_info)
$(show_uptime)
$(drive_space)
$(home_space)
$(sys_info)

$TEXT_GREEN$TIME_STAMP$TEXT_RESET
_EOF_
}

# Procesar la línea de comandos del script para leer las opciones
while [ "$1" != "" ]; do
  case $1 in
    -f | --file )
      shift
      filename=$1
      ;;
    -i | --interactive )
      interactive=1
      ;;
    -h | --help ) 
      usage
      exit
      ;;
    * )
      usage
      error_exit "$LINENO: Opción no soportada: $1"
      ;;
  esac
  shift
done

type -P df || error_exit "$LINENO: No se ha encontrado el comando df."

type -P du || error_exit "$LINENO: No se han encontrado el comando du."

type -P uptime || error_exit "$LINENO: No se ha encontrado el comando uptime."

# Generar el informe del sistema y guardarlo en el archivo indicado
# en $filename
if ["$interactive" = "1" ]; then
  interactive_mode
fi

write_page > $filename





