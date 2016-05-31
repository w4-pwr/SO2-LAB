#Kasowanie wszystkich plików pustych (o zerowej wielkości) w zadanym katalogu (parametr
#wywołania skryptu). Skrypt powinien tworzyć w zadanym pliku listę skasowanych plików.
#Nie powinien analizować dołączeń symbolicznych.h.

#!/bin/bash
clear
echo "Zadanie 9"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
  exit 1
fi

for file in $1
do

	#-L returns true if the "file" exists and is a symbolic link (the linked file may or may not exist)
	#-s false if empty
	#-s wielkosc -f czy plik -L czy dowiązanie symboliczne
        if [[ ! -s $plik ]] && [[ -f $plik ]] && [[ ! -L $plik ]]
        then
                echo "Usunąłem plik $plik." >> $biezacy_katalog/$2
                rm $plik -f
        fi
done
