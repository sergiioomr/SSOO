# #!/usr/bin/env bash

# Lista para almacenar los usuarios pasados por parámetro
users_list=
min_time=
# Opción sin parámetros

show_process() {
    if [ "$users" = 1 ]; then
        for user in "$users"; do
            ps axu | awk '$1 == "$user" {print$1} ' | uniq -c
        done
    else
        ps axu | tail -n+2 | awk '{print$1}' | sort | uniq -c | sort -k2
    fi
}

# Procesamiento de parámetros
while [ "$1" != "" ]; do
    case $1 in
        -u )
        users=1
        shift
        while [ [ "$1" != -* ] && [ "$1" != "" ] ]; do
            users_list="$users_list $1"
            shift
        done
        ;;
        -m )
        shift
        min_time=$1

        ;;
        -n )

        ;;
        * )
        usage
        exit_error
        ;;
    esac
done
