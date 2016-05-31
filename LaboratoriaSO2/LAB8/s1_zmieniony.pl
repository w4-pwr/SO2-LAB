#!/usr/bin/perl

open($PLIK, $ARGV[0]) || die "Nie można otworzyć pliku";

while($line = <$PLIK>)
{
         $line =~ s/[Aa][Ll][Aa]*./Ala/g;
	print("\n$line");
}

