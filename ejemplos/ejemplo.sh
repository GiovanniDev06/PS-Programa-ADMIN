#!/bin/bash

# Script de ejemplo para probar el analizador

contador=0
nombre="admin"
total=10

for i in 1 2 3 4 5
do
    contador=$((contador + 1))
    echo "Iteracion: $i"
done

while [ $contador -lt $total ]
do
    contador=$((contador + 1))
done

echo "Contador final: $contador"
