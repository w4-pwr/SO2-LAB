#Łączenie w jeden wszystkich plików należących do zadanego katalogu (parametr wywołania
#skryptu), o zadanym rozszerzeniu (drugi parametr skryptu). Kolejność, w jakiej pliki zostaną
#połączone - nieistotna. Treść każdego pliku powinna być poprzedzona nagłówkiem z jego
#nazwą.

#!/bin/bash

clear
echo "Zadanie 7"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

source=$1
destination=$2

for file in $source/*
do
	echo "Plik: $file"
	if [ -x $file ]
	then	
		mv $file $destination
	fi
done


fi

