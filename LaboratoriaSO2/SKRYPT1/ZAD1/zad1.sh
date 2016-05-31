#Zmiana nazw wszystkich plików w zadanym katalogu (parametr wywołania skryptu), do
#których mamy ustawione prawo zapisu, przez dopisanie dodatkowego członu .old. Wcześniej
#należy skasować wszystkie pliki, które już mają takie rozszerzenie.

#!/bin/bash

echo "Zadanie 1"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

#echo "Parametr do $1"
cd $1

for file in *
do

	if [[ $file == *".old" ]]
	then
		rm  $file	
	fi

	if [ -w $file ] && [ -e $file ]
	then	
		mv "$file" `echo "$file".old`
	fi
done

fi

