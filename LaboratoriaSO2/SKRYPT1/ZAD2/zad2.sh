#Tworzenie nowych plików w zadanym katalogu (parametr wywołania skryptu), według listy
#umieszczonej w pliku (drugi parametr wywołania skryptu). Nowe pliki mają być zerowej
#wielkości (puste). Jeżeli jakiś plik już istnieje, to nie powinien zostać zniszczony

#!/bin/bash

echo "Zadanie 2"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

sourceList="$2"
while read -r readedLine
do
	echo "$readedLine"
	touch $1/$readedLine
done < "$sourceList"


fi

