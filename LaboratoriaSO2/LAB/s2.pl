#!/usr/bin/perl


open(PLIK , "$ARGV[0]") || die "Nie mozna otworzyć pliku" ;

while($linia = <PLIK> )
{
	@spl = split(" ",$linia);
	@matches = grep /\d{$ARGV[1]}/,@spl;
	foreach $d (@matches)
	{
		print("$d\n");
	}
}
