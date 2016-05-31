#Kasowanie wszystkich plików pustych (o zerowej wielkości) w zadanym katalogu (parametr
#wywołania skryptu). Skrypt powinien tworzyć w zadanym pliku listę skasowanych plików.
#Nie powinien analizować dołączeń symbolicznych.

#!/bin/bash
clear
echo "Zadanie 9"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
  exit 1
fi

#pierwszy parametr katalog gdzie skasowac pliki
echo "" > $1/listaSkasowanych.txt

for file in `ls $1`
do
	if [ ! -s $1/$file ] && [ ! -h $1/$file ]
	then
		echo $file >> $1/listaSkasowanych.txt
		rm -f $1/$file
	fi 
done
exit 0