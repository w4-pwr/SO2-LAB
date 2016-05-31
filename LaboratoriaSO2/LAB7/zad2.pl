#!/usr/bin/perl

open(FILE, $ARGV[0]);

$n = $ARGV[1];

while($line = <FILE>){
	#jedna linia z pliku tekstowego
	@array = split (/\s/, $line); #podzielone po spacji
	$arr_size = @array;
	for($i = 0; $i < $arr_size; $i++){
		$zdanie = $array[$i];
		#sprawdzic czy jest to liczba 
		$zdanie =~ s/([^0-9])//g;

		if($zdanie > $n){
			print "$zdanie\n";
		}
	}
}