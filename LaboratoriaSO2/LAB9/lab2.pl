#!/usr/bin/perl

#zad 2 - wylistowac pliki do ktorych moge pisac, ale nie moge usuwac
use File::Find;
use File::stat;

find(\&statOfTree, $ARGV[0]);


sub statOfTree {
	$file = $_;

	#czy katalog
	if(-d $file){
		#brak prawa do zapisu (czyli usuwania z katalogu)
		if(!-w $file){
			#katalog z ktorego nie mozna usuwac
			find(\&listFilesWithWritePermision, $file);
		}
	}
}

sub listFilesWithWritePermision(){
	$file = $_;
	if(-w $file){
		print "$file\n";
	}
}
