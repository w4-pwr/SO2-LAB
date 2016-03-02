#Ustawienie na aktualny czasu ostatniej modyfikacji wszystkich plików zadanego katalogu
#(parametr wywołania skryptu), do których mamy prawo dostępu do zapisu/modyfikacji.

#!/bin/bash

echo "Zadanie 14"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

for file in $1/*
do
	if [[ -w $file ]] #jeszcze jakies sprawdzenie?
	then 
		touch $file
	fi
done

fi

