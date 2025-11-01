#!/usr/bin/env bash
# Variables globales
regexp=
# 1. Funcionamiento básico y listado de usuarios con archivos abiertos.


# Posible comando funcional -> lsof | awk '{}'

## Procesamiento de parámetros

while [ "$1" != "" ]; do
  case $1 in
    -h | --help )
      usage
      exit
      ;;
    -f )
      shift
      regexp=$1
      ;;
    -o | --offline )
      #...
      ;;
    -u )
      #...
      ;;
    -s | --sort )
      #...
      ;;
    * )
      usage
      exit 1
      ;;
  esac
  shift
done


