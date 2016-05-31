#!/usr/bin/perl
use File::Find;
use File::stat;

$fileNearOffset = 99999999999;
$larggestFileSize = 0;
$larggestFile;
$samllestFileSize;
$firstIteration = 1;
$smallestFile;
$filesCount = 0;
$overallSizeOfFiles = 0;


find(\&statOfTree, $ARGV[0]);


sub statOfTree {
	$file = $_;
	$sizeOfFile = stat($file)->size;

	#print("Plik: $file \t\t ($sizeOfFile bajtów) \n");

	#jesli jest to plik regularny
	if (-f $file) {

		$filesCount++;
		$overallSizeOfFiles += $sizeOfFile;

		#pierwszy plik jako najmnijeszy
		if($firstIteration == 1){
			$firstIteration = 0;
			$smallestFile = $file;
			$samllestFileSize = $sizeOfFile;
		}

		#jesli obecny plik jest wiekszy od najwiekszego obecnie
		if($sizeOfFile > $larggestFileSize) {	
			$larggestFileSize = $sizeOfFile;	
			$larggestFile = $file;
		}

		#jesli obecny plik jest mniejszy od najmniejszego obecnie
		if($sizeOfFile < $samllestFileSize) {
			$samllestFileSize = $sizeOfFile;
			$smallestFile = $file;
		}
	}
}



$fileNear;
$fileNearSize;

$avg=$overallSizeOfFiles/$filesCount;
find(\&choseFile, $ARGV[0]);
sub choseFile {
	$file = $_;
	$sizeOfFile = stat($file)->size;

	if (-f $file) {
		$fileNearOffset = $larggestFileSize / 2;
		if($fileNearOffset < $larggestFileSize){
			$fileNearOffset = $sizeOfFile;
			$fileNear = $file;
			$fileNearSize = $sizeOfFile;
		}
	}

}


print("Najwiekszy:\t$larggestFile rozmiar:\t$larggestFileSize bajtów\n");
print("Najmniejszy:\t$smallestFile rozmiar:\t$samllestFileSize bajtów\n");
print("Srednia wielkosc wszystkich plikow:\t$avg bajtów\n");
print("Najblizej tej wielkosci jest plik:\t$fileNear rozmiar:\t$fileNearSize bajtów\n");


#srednia plikow 
#i podac plik ktory jest najblizej tej sredniaj 