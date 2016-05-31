#!/bin/bash
clear
echo "sciezka: $1" 

if [ ! -e $1 ]; then
	echo "symlink zepsuty :c"
else
	echo "symlink spoko? c:"
fi
