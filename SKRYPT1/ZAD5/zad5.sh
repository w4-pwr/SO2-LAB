#Łączenie w jeden wszystkich plików należących do zadanego katalogu (parametr wywołania
#skryptu), o zadanym rozszerzeniu (drugi parametr skryptu). Kolejność, w jakiej pliki zostaną
#połączone - nieistotna. Treść każdego pliku powinna być poprzedzona nagłówkiem z jego
#nazwą.

#!/bin/bash

clear
echo "Zadanie 5"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

mergedFile=$1/polaczone.$2


for file in $1/*
do

	if [ ! -e $mergedFile ]
	then	
		touch mergedFile
	fi

	echo "Nazwa pliku: $file" >> $mergedFile

	while read -r readedLine
	do
		echo "$readedLine" >> $mergedFile
	done < "$file"
done


fi

