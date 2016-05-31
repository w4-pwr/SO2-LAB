#!/usr/bin/perl

open( FILE, "$ARGV[0]") || die ("Nie mozna otworzyc pliku");
while ($line = <FILE>)
{
	@tab = split (/[?!.]/, $line);
	print("$tab[0]\n");
	for ($i=1;$i<@tab;$i++)
	{
		$tab[$i] =~ s/ //; 
		print("$tab[$i]\n");
	}
}
