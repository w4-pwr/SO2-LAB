#!/usr/bin/perl

open(FILE, $ARGV[0]);

while($line = <FILE>){
	#jedna linia z pliku tekstowego
	@array = split (/[.?!]/, $line); #podzielone na zdania
	$arr_size = @array;
	for($i = 0; $i < $arr_size; $i++){
		$zdanie = $array[$i];

		$zdanie =~ s/^\s+//;

		# @podzielone_zdanie = split(//, $zdanie);
		# for($j = 0; $j < @podzielone_zdanie; $j++){
		# 	$znak = $podzielone_zdanie[$j];
		# 	if($znak != (/ /)){
		# 	print "$znak";
		# }
		# }

		$zdanie =~ s/([0-9]+)/"$1"/g;

		print "$zdanie\n";

	}



}