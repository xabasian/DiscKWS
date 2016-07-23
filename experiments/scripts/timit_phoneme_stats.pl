#!/usr/bin/perl -w

unless ($#ARGV+1 == 3) {
    print "Usgae: $0  <phn file list> <phonemes-to-index map> <61-to-39 phoneme map>\n";
    exit;
}

$file_list = $ARGV[0];
$phoneme_to_index_file = $ARGV[1];
$phonemes_61_to_39_file = $ARGV[2];

# read label map file into hash array
%phoneme2index = ();
$num_phonemes = 0;
open(PHONEMES, $phoneme_to_index_file) or die "Can't open $phoneme_to_index_file:$!\n";
while (<PHONEMES>) {
    chomp;
	($phoneme_code,$phoneme_index) = split;
    $phoneme2index{$phoneme_code} = $phoneme_index;
	$num_phonemes++;
}
close(PHONEMES);

# read label map file into hash array
%phoneme61_to_phoneme39 = ();
open(PHONEMES, $phonemes_61_to_39_file) or die "Can't open $phonemes_61_to_39_file:$!\n";
while (<PHONEMES>) {
    chomp;
	($phoneme_61code,$phoneme_39code) = split;
    $phoneme61_to_phoneme39{$phoneme_61code} = $phoneme_39code;
}
close(PHONEMES);

# define array that stores the average length of each phoneme
for ($i = 0; $i < $num_phonemes; $i++) {
    $n[$i] = 0;
    # Shima code
    $n1[$i]=0;
    $n2[$i]=0;
    $n3[$i]=0;
    $n4[$i]=0;
    $n5[$i]=0;
    $n6[$i]=0;
    $n7[$i]=0;
    $n8[$i]=0;
    $n9[$i]=0;
    $StartValue=1;
    $EndValue=45;
    $RangeLength=5;
    $TempVal=0;
    ############
    $average[$i] = 0;
    $std[$i] = 0;
    $log_average[$i] = 0;
    $min_len[$i] = 9999999999;
    $max_len[$i] = 0;
}

# Run over the file list
open(FILELIST,"$file_list") or die "Error: unable to open $file_list $!\n";
while (<FILELIST>) {
    chomp;
    open(PHNFILE, $_) or die "Cannot open file $_: $!\n";
    while (<PHNFILE>) {
	chomp;
	($p_start, $p_stop, $p_code) = split;
	$p_start = $p_start/160.0; # convert samples (@ 16kHz) to 10msec frames
	$p_stop = $p_stop/160.0; # convert samples (@ 16kHz) to 10msec frames
	$d = $p_stop-$p_start+1;
	$c = $phoneme2index{$phoneme61_to_phoneme39{$p_code}};
	$n[$c]++;
	$average[$c] = ( ($n[$c]-1)*$average[$c] + $d )/$n[$c];
	$log_average[$c] = ( ($n[$c]-1)*$log_average[$c] + log($d) )/$n[$c];
	$std[$c] = ( ($n[$c]-1)*$std[$c] + $d**2 )/$n[$c];
	if ($d < $min_len[$c]) {
	    $min_len[$c] = $d;
	}
	if ($d > $max_len[$c]) {
	    $max_len[$c] = $d;
	}
        #Shima code#############################################################
        if(($d>=$StartValue) && ($d<$StartValue+$RangeLength)){
            $n1[$c]++;
        }
        if(($d>=$StartValue+$RangeLength) && ($d<$StartValue+2*$RangeLength)){
            $n2[$c]++;
        }
        if(($d>=$StartValue+2*$RangeLength) && ($d<$StartValue+3*$RangeLength)){
            $n3[$c]++;
        }
        if(($d>=$StartValue+3*$RangeLength) && ($d<$StartValue+4*$RangeLength)){
            $n4[$c]++;
        }
        if(($d>=$StartValue+4*$RangeLength) && ($d<$StartValue+5*$RangeLength)){
            $n5[$c]++;
        }
        if(($d>=$StartValue+5*$RangeLength) && ($d<$StartValue+6*$RangeLength)){
            $n6[$c]++;
        }
        if(($d>=$StartValue+6*$RangeLength) && ($d<$StartValue+7*$RangeLength)){
            $n7[$c]++;
        }
        if(($d>=$StartValue+7*$RangeLength) && ($d<$StartValue+8*$RangeLength)){
            $n8[$c]++;
        }
        if(($d>=$StartValue+8*$RangeLength) && ($d<=$EndValue)){
            $n9[$c]++;
        }
        ##########################################################################
    }
    close(PHNFILE);    
}
close(FILELIST);

print "41 $num_phonemes\n"; # infra textual format
#print "39 9\n"; # infra textual format
for ($i = 0; $i < $num_phonemes; $i++) {
    print "$average[$i] ";
}
print "\n";
for ($i = 0; $i < $num_phonemes; $i++) {
  $std[$i] = sqrt($std[$i] - $average[$i]**2);
  print "$std[$i] ";
}
print "\n";
# #commends to print minimum and maximum length of each phoneme
# for ($i = 0; $i < $num_phonemes; $i++) {
#     print "$min_len[$i] ";
# }
# print "\n";
# for ($i = 0; $i < $num_phonemes; $i++) {
#     print "$max_len[$i] ";
# }
# print "\n";
#commends to print Histograms for each phoneme
for ($i = 0; $i < $num_phonemes; $i++) {
     $TempVal=$n1[$i]/$n[$i];
     print "$TempVal ";
     $TempVal=$n2[$i]/$n[$i];
     print "$TempVal ";
     $TempVal=$n3[$i]/$n[$i];
     print "$TempVal ";
     $TempVal=$n4[$i]/$n[$i];
     print "$TempVal ";
     $TempVal=$n5[$i]/$n[$i];
     print "$TempVal ";
     $TempVal=$n6[$i]/$n[$i];
     print "$TempVal ";
     $TempVal=$n7[$i]/$n[$i];
     print "$TempVal ";
     $TempVal=$n8[$i]/$n[$i];
     print "$TempVal ";
     $TempVal=$n9[$i]/$n[$i];
     print "$TempVal ";
     for($j=0;$j<30;$j++){
         print "0 ";
     }
     print "\n";
}