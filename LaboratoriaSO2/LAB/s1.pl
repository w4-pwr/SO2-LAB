#!/usr/bin/perl

$mask = $ARGV[1];
$mask =~ s/^([a-z])/\u$1/;
$fileName = $ARGV[0];
$pattern = $ARGV[1];

open(file , $fileName);

while($line = <file>) {
	@arrayOfWords = split(" ",$line); #jeden wyraz

	#/i - nie zwraca uwagi na wielkosc liter
	#\b - granica słowa -> /\bnie\b/; tam gdzie wystepuje 'nie'
	@arrayOfMatchedWordsToPattern = grep /$pattern/i, @arrayOfWords; # wszystkie wyrazy które pasują z listy
	$line =~ s/$pattern/$mask/gi;
	print("$line\n");
}
