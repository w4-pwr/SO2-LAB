#!/usr/bin/perl

$toUpperCase = $ARGV[1];
$toUpperCase =~ s/^([a-z0-9])/\u$1/;
# ^	   - zaczyna sie od
#[a-z] - male litery
#() - zapis 
#\u duza litera

$fileName = $ARGV[0];
$inputWord = $ARGV[1];
$wordsToChange = 0;
$goodWordsSize = 0;

$countChanged = 0;
$countNotChanged = 0;
$allWords = 0;

open(file, $fileName);

while($line = <file>) {

	#oddzielone wszystkim tylko nie znakmi alfabetu
	@arrayOfWords = split(/[^\w|\d]/,$line); #lista wyrazów
	#tam gdzie sa gdzie liczby zmieniam 
	#lista co zmienilem a czego nie

	@arrayOfGoodWords = grep /$inputWord/, @arrayOfWords;

	$allWords += @arrayOfWords;
	
	foreach(@arrayOfWords){
		$word = $_;

		#zawiera cyfry -> zmieniam
		if ($word =~ /[\d]/)
		{	
			$countChanged++;
			$word =~ s/^([a-z])|[0-9]/\u$1/;
			print "Zmieniłem wyraz na: $word\n";
		} else {
			print "Nie zmieniłem: $word\n";
		}
	}

	$goodWordsSize += @arrayOfGoodWords;

	@arrayOfMatchedWordsToPattern = grep /$inputWord/i, @arrayOfWords; 

	$wordsToChange += @arrayOfMatchedWordsToPattern;
}

$count = $wordsToChange - $goodWordsSize;
$countNotChanged = $allWords - $countChanged;
#print("Liczba: $allWords\n");
print("Nie zmieniłem: $countNotChanged\n");
print("Zmieniłem: $countChanged\n");