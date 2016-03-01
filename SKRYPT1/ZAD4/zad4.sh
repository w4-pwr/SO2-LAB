#Numerowanie wszystkich plików w zadanym katalogu (parametr wywołania skryptu), do
#których mamy ustawione prawo wykonywania („execute”), przez dodanie dodatkowego
#członu rozszerzenia o postaci .<numer kolejny>. Numeracja powinna przebiegać według
#wielkości plików. 

#!/bin/bash

clear
echo "Zadanie 4"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
else

counter=0

for file in `ls -S $1/*`
do

	echo "Wielkosc: `cat $file | wc -c` pliku $file"
	
	if [[ -x $file ]]
	then
		echo "Plik jest wykonywalny, zmieniam nazwę"
		mv "$file" `echo "$file"$counter`
		((counter++))
	fi

done


fi

