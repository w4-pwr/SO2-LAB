#Porównanie zawartości dwóch zadanych katalogów (argumenty skryptu). Przy porównaniu
#należy ignorować podkatalogi. W wyniku wyświetlić na ekranie listę plików o identycznych
#nazwach w obu katalogach.

#!/bin/bash
clear
echo "Zadanie 10"

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
  exit 1
fi

for file1 in `ls $1`
do
        for file2 in `ls $2`
        do
                if [[ ! -d $1/$file1 ]] && [[ ! -d $2/$file2 ]] #ignorowanie katalogow
                then
                        if `cmp --silent $1/$file1 $2/$file2`
                        then
                                echo $file1 >> tmp.txt
                        fi
                fi
        done
done

cat tmp.txt | uniq
