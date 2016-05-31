
#!/bin/bash
clear
echo "Zadanie "

if [ $# -eq 0 ]
then
  echo "Brak parametrów"
  exit 1
fi

filesList=$2
dstDir=$1

#czytanie z pliku
while read readedLine
do
	echo "$readedLine"
	touch $dstDir/$readedLine
done < $filesList

#przegląd plików
for file in `ls $1`
do
	if [ ! -s $1/$file ] && [ ! -h $1/$file ]
	then
		echo $file >> $1/listaSkasowanych.txt
		rm -f $1/$file
	fi 
done

exit 0
