#!/usr/bin/perl

open(PLIK, $ARGV[0]);

while($line = <PLIK>)
{
	@tab = split (/[!.?]/, $line);
	for ($i=1; $i < @tab; $i++)
	{
		$zmienna = $tab[$i];
		$temp = reverse($zmienna);
		chop ($temp);
		$zmienna = reverse($temp);
		print("$zmienna \n");
	}
}
