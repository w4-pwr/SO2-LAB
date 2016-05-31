#Kasowanie wszystkich plików w zadanym katalogu (parametr wywołania skryptu), poza
#plikami wykonywalnymi, mającymi ustawiony bit dostępu typu „execute”. 

#!/bin/bash

echo "Zadanie 3"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

for file in $1/*
do

	echo "Znalazlem plik $file"

	if [[ ! -x $file ]]
	then
		echo "Plik nie jest wykonywalny, usuwam $file"
		rm  $file	
	fi

done


fi

