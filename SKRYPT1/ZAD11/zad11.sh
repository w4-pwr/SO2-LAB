#Porównanie zawartości zadanego katalogu z listą plików (nazwa katalogu i pliku z listą
#zadawana w argumentach skryptu). Należy wygenerować listę plików brakujących w
#katalogu i takich, które nie są na liście.

#!/bin/bash
clear
echo "Zadanie 11"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
  exit 1
fi

#pierwszy arg 	katalog
#drugi arg 	lista

touch brakujaceWKatalogu.txt
for fileFromList in `cat $2`
do
        isFound=false
        for fileInDir in `ls $1`
        do
                if [[ $fileFromList = $fileInDir ]]
                then
                	isFound=true        
                fi
        done
        if ! $isFound
        then
        	echo $fileFromList >> brakujaceWKatalogu.txt
        fi
done

touch brakujaceNaLiscie.txt
for fileInDir in `ls $1`
do
        isFound=false
        for fileFromList in `cat $2`
        do
                if [[ $fileFromList = $fileInDir ]]
                then
                	isFound=true        
                fi
        done
        if ! $isFound
        then
        	echo $fileInDir >> brakujaceNaLiscie.txt
        fi
done
