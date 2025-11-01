#!/usr/bin/env bash



ordenproc=1
nombreusuario=""
tiempoMin=""

procesos() {  # Funcion principal que hace todos los ccalculos



if [ "$nombreusuario" = "" ]   # si no se han activado los nombres de usuario se generan como el listado de usuarios que tienen procesos
then
	nombreusuario=$(ps -A -o user --no-header)
fi



for i in $(echo $nombreusuario | tr ' ' '\n' | sort -u)  # el listado se ordena, y se quitan los repetidos
do

	nprocesos=0

	for t in $(ps --no-header -o time -u $i)  # por cada usuario se busca el tiempo en formato 00:00:00
	do		
		if [[ "$t" > "$tiempoMin" ]]   # miramos el tamaño, en el caso de no indicar ninguno, tiempoMin esta vacio, por lo $t siempre es mayor
		then		
			nprocesos=$((nprocesos+1))			
		fi
	done
	printf "%-20s  %-20s\n" "$i $nprocesos"
done
}


while [ "$#" -gt 0 ]; do
    case $1 in
    -u) usuario=1
    shift
    while [[ $1 != -* ]] && [ -n "$1" ]   # capturamos el listado de usuarios y lo ponemos en un lista
    do    	
    	nombreusuario="$nombreusuario $1" 
    	shift
    done
   
    ;;
    -m) shift  # almacenamos el tiempo minimo, ya tiene el valor vacio por defecto
    tiempoMin=$1
   
    ;;
    -n) ordenproc=2  # indicamos que hay que ordenar por num procesos, se guarda en el formato 00:00:00
    
    ;;
    
    *) echo "Error: parametro no valido" $1
    	exit 1
    ;;
    esac
    shift  
done

printf "%-20s  %-20s\n" "USUARIOS PROCESOS"	

procesos | sort -k$ordenproc -n   # dependiendo del parametro se ordena de una forma u otra directamente


