#Listowanie plików z zadanego katalogu (parametr wywołania skryptu), wraz z jego
#podkatalogami. Zawartość podkatalogów ma być listowana w postaci <ścieżka dostępu
#względem listowanego katalogu>/<nazwa pliku>. Ograniczyć liczbę zagnieżdżeń podkatalogów
#(np. do dwóch)

#!/bin/bash

counter=0

function list {
startListing=$1
for file in $startListing/*
do
	echo "$file"
	if [[ -d $file ]] && [[ counter -lt 2 ]]
	then
		((counter++))
		list $file
	fi
done
}

clear
echo "Zadanie 8"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

list $1

fi



