#Skrypt zlicza wszystkie pliki w zadanym katalogu (parametr wywołania skryptu), do których ustawione jest prawo do wykonania (execute)

#!/bin/bash

echo "Zadanie 15"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

counter=0
for file in $1/*
do
        if [[ -x $file ]]
        then
                ((counter++))
        fi
done

echo "Liczba plików: $counter"
