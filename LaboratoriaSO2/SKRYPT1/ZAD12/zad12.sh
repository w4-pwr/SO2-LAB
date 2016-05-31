#Usuwanie wszystkich podkatalogów zadanego katalogu (parametr wywołania skryptu).
#Zawartość tych podkatalogów należy przenieść do katalogu nadrzędnego. Usuwanie dotyczy
#tylko jednego poziomu podkatalogów.

#!/bin/bash
clear
echo "Zadanie 12"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
  exit 1
fi

for file in `ls $1` #czemu nie dziala zwykle $1/* ?
do
	echo "Plik $file"

        if [[ -d $1/$file ]]
        then
		echo "to katalog"
                mv $1/$file/* $1
                rm -d $1/$file
        fi
done



