#Tworzenie nowych plików w zadanym katalogu (parametr wywołania skryptu), według listy
#umieszczonej w pliku (drugi parametr wywołania skryptu). Nowe pliki mają być zerowej
#wielkości (puste). Jeżeli jakiś plik już istnieje, to nie powinien zostać zniszczony.

#!/bin/bash
clear
echo "Zadanie 2"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
  exit 1
fi

#pierwszy parametr z katalogiem gdzie tworzyć pliki
#drugi parametr - lista z plikami

filesList=$2
dstDir=$1

while read readedLine
do
	echo "$readedLine"
	touch $dstDir/$readedLine
done < $filesList

exit 0