#!/bin/bash
#w zadanym drzewie katalogów znalexc dowiazania symboliczne wskazujace na nieistniejace obiekty


if [ $# -eq 0 ]
then 
	echo "brak argumentow"
	exit 1
fi

dirParam=$1

for file in $dirParam/*
do	
	if [[ -h $file ]] #czy dowiązanie symboliczne
	then	
		realPath=`readlink $file`
		
		if [[ ! -e $file ]] # nie istnieje
		then
			echo "Nie istnieje dowiazanie dla: $file" 
		fi

	fi
done


exit 0
