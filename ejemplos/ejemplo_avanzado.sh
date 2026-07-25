#!/bin/bash
# Script de prueba avanzado - casos limite

contador=0
nombre="Dario Cornejo"
ruta=/home/usuario/admin
lista=(1 2 3 4 5)

export PATH_ADMIN=/opt/admin
local temporal=5
readonly MAX_INTENTOS=3
declare -i puntaje=100
declare -x MODO=produccion

# Esto es un comentario con variable=falsa, no debe detectarse
    # Comentario indentado, tampoco debe detectarse

contador+=1

if [ "$nombre" == "admin" ]; then
    echo "coincide"
fi

if [[ $contador -eq 0 ]]; then
    echo "cero"
fi

for i in 1 2 3 4 5
do
    contador=$((contador + 1))
done

for (( j=0; j<10; j++ ))
do
    echo "$j"
done

while [ $contador -lt $MAX_INTENTOS ]
do
    contador=$((contador + 1))
done

until [ $contador -ge 20 ]
do
    contador=$((contador + 1))
done

for k in $(ls); do
    echo "$k"
done
