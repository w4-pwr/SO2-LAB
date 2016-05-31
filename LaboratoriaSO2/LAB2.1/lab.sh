#!/bin/bash
#w zadanym katalogu znalezc dowiazania symboliczne 
#bedace dowiazaniami symbolicznymi do podkatalogow tego samego katalogu
clear
if [ $# -eq 0 ]
then 
	echo "brak argumentow"
	exit 1
fi

dirParam=$1

for file in $dirParam/*
do	
	if [[ -L $file ]] #czy dowiazanie symboliczne
	then	
		realPath=`readlink -f $file`
		if [ -d $file ] #czy katalog
		then
			dirName=`dirname $realPath`
			if [[ $dirParam == $dirName ]]
			then
				echo "To ten: $file"
			fi

		fi
	fi
done


exit 0
