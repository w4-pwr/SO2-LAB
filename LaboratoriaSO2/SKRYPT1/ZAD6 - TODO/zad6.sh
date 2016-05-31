#Łączenie w jeden plików z listy, umieszczonej w pliku o zadanej nazwie (parametr
#wywołania skryptu. Kolejność, w jakiej pliki zostaną połączone - ściśle według listy. Treść
#każdego pliku powinna być poprzedzona nagłówkiem z jego nazwą. Plik wynikowy powinien
#mieć nazwę pliku pierwotnie zawierającego listę. 

#jeden parametr?

#!/bin/bash

clear
echo "Zadanie 6"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

filesList=$1


for file in $1/*
do
	while read -r readedLine
	do
		echo "$readedLine" >> $filesList
	done < "$file"
done


fi

