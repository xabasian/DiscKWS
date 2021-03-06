#!/usr/bin/perl -w

unless ($#ARGV+1 == 6) {
	print "Usgae: $0  <keyword list (in)> <input file list (in)>\n".
	"<positives files (out)> <negative files (out)>\n".
	"<keyword phoneme list (out)> <keyword alignemnt list (out)>\n";
	exit;
}

$keyword_list = $ARGV[0];
$input_file_list = $ARGV[1];
$positive_files = $ARGV[2];
$negative_files = $ARGV[3];
$keyword_phoneme_list = $ARGV[4];
$keyword_alignment_list = $ARGV[5];

# set timit location
open(TIMIT_LOCATION,"config/timit_location") or die "Error: unable to open config/timit_location $!.\n".
"This file was supposed to be generated by scripts/q1-extract_features_and_labels.sh\n";
$timit_original_dir = <TIMIT_LOCATION>;
chomp $timit_original_dir;
close(TIMIT_LOCATION);
#$timit_original_dir = "/Corpora/timit";

# read training file names
print "Building a list of training files... ";
open(INPUT_FILE_LIST,"$input_file_list") 
or die "Error: unable to open $input_file_list $!\n";
@training_filenames = ();
	print "ooooooooooooooooooooooy1";
while (<INPUT_FILE_LIST>) {


	chomp;
	push @training_filenames, $_.".mfc";
}
close(INPUT_FILE_LIST);

print "  done.\n";

# load prompts file
print "Loading prompts... ";
$timit_prompts = $timit_original_dir."/DOC/PROMPTS.TXT";
open(PROMPTS_FILE,"$timit_prompts") or die "Error: unable to open $timit_prompts $!\n";
while (<PROMPTS_FILE>) {
	chomp;
	s/;.*//;                # no comments
	s/^\s+//;               # no leading white
	s/\s+$//;               # no trailing white
	next unless length;     # anything left?
	@timit_words = split;
	$file_code = pop @timit_words;
	$file_code =~ s/^\((.*)\)$/$1/;
	$joint_words = join(" ", @timit_words);
	$joint_words =~ s/\.//g; # remove points
	$joint_words = lc($joint_words); # lower case
	$prompts{$file_code} = $joint_words;
}
close(PROMPTS_FILE);
print "  done.\n";

# load phone map (61 timit phone set to 39 lee-hon set)
%map = ();
$label_map_file = "config/phoneme_map_timit61_to_leehon39";
open(LABELS, $label_map_file) or die "Can't open $label_map_file:$!\n";
while (<LABELS>) {
	chomp;
	($label1,$label2)=split(" ");
	$map{$label1} = $label2;
}
close(LABELS);


# read keywords 
print "Reading keyworks... ";
open(KEYWORD_LIST_FILE,"$keyword_list") or 
die "Error: unable to open $keyword_list $!\n";
@keywords = <KEYWORD_LIST_FILE>;
close(KEYWORD_LIST_FILE);
print "  done.\n";


# open output files
open(TRAIN_NEG, ">$negative_files") or 
die "Error: unable to open $negative_files $!\n";
open(TRAIN_POS, ">$positive_files") or 
die "Error: unable to open $positive_files $!\n";
open(KEYWORD_PHONEME_LIST, ">$keyword_phoneme_list") or 
die "Error: unable to open $!\n";
open(KEYWORD_ALIGNMENT_LIST, ">$keyword_alignment_list") or
die "Error: unable to open $!\n";

# run over all keywords
foreach $keyword (@keywords) {
	chomp $keyword;

	next if ($keyword eq "");
	print "---\n";
	print "$keyword\n";
	if ($keyword !~ /^[A-Za-z]+$/) {
		print "  skipping...\n";
		next;
	}

	# find all files containing keywords (positive files)
	@positive_file_list = ();
	foreach $file_code (keys %prompts) {
					
		if ($prompts{$file_code} =~ /\b$keyword\b/) { # \b matches word boundaries
			foreach $file (@training_filenames) {
					
				if ($file =~ m/$file_code\./) {
#die "Error: unable to upen m/$file_code\./ ";
					push @positive_file_list, $file;

				}
			}
		}
	}
	# if no files were found skip to the next keyword
	if (scalar(@positive_file_list) < 1) {
		print "  skipping...\n";
		next;
	}
	# random shuffle all positive files
	fisher_yates_shuffle(\@positive_file_list);

	# find all files the does not contain the keyword (negative files)
	my %seen; # lookup table
	my @negative_file_list; # answer
	@seen{@positive_file_list} = ();       # build lookup table
	foreach $file (@training_filenames) {
		push(@negative_file_list, $file) unless exists $seen{$file};
	}
	# random shuffle all negative files
	fisher_yates_shuffle(\@negative_file_list);

	# print to files only the first positive file and the first negative file
	$j = 0;
	$positive_file_list[$j] =~ s/\.mfc//;
	$negative_file_list[$j] =~ s/\.mfc//;
	print TRAIN_POS "$positive_file_list[$j]\n";
	print "pos: $positive_file_list[$j]\n";
	print TRAIN_NEG "$negative_file_list[$j]\n";
	print "neg: $negative_file_list[$j]\n";
	
	# load phoneme list and start time list
	$phonemes_file = $positive_file_list[$j].".phonemes";
	@phonemes = ();

	open(PHONEMES, $phonemes_file) or die "Can't open $phonemes_file:$!\n";

	while (<PHONEMES>) {
		chomp;
		push @phonemes, $_;
	}
	close(PHONEMES);
	$start_times_file = $positive_file_list[$j].".start_times";
	@start_times = ();
	open(START_TIMES, $start_times_file) or die "Can't open $start_times_file:$!\n";

	while (<START_TIMES>) {
		chomp;
		push @start_times, $_;
	}
	close(START_TIMES);
	$start_times_file = $positive_file_list[$j].".scores";
	#die "Error: unable to uupen $start_times_file";
	open(START_TIMES, $start_times_file) or die "Can't open $start_times_file:$!\n";

	$line = <START_TIMES>;
	my $num_features;
	($num_frames, $num_features) = split(/ /, $line);
	close(START_TIMES);

	# find phonetic transcription and alignment of the keyword
	$j = 0;
	$timit_wrd_file = $positive_file_list[$j];
	$timit_wrd_file =~ s/data/$timit_original_dir/;    
	$timit_wrd_file .= ".wrd";
	$timit_phn_file = $timit_wrd_file;
	$timit_phn_file =~ s/.wrd/.phonemes/;
	# find the begin and end of the keyword
	open(SYSTEMCALL,"grep -w $keyword $timit_wrd_file |") 
	or die "Error: unable to grep wrd file $!\n";
	$line = <SYSTEMCALL>;
	my $keyword2;
	($begin, $end, $keyword2) = split(" ", $line);
	# find the list of phonemes 
	@keyword_phonemes = ();
	$found_begin = 0;
	open(PHNFILE,"$timit_phn_file") or die "Error: unable to open $timit_phn_file $!\n";
	while (<PHNFILE>) {
		chomp;
		if ($found_begin == 0) {
			next if ($_ !~ m/$begin/);
			$found_begin = 1; 
		}
		else {
			my ($phone_begin, $phone_end, $phone_code);
			($phone_begin, $phone_end, $phone_code) = split;
			if (!exists $map{$phone_code}) { die "Error: phone $phone_code not found in the map\n"; }
			push @keyword_phonemes, $map{$phone_code};
			last if ($phone_end >= $end); #last if ($_ =~ m/$end/);
		}
	}
	close(PHNFILE);

	$first_phoneme_loc = 0;
	$first_occurance = 1;
	$num_phonemes_found = 0;
	$j = 0; 
	$i = 0;
	while ($i < scalar(@phonemes)) {
		#print "$i $phonemes[$i]  $keyword_phonemes[$j] $num_phonemes_found\n"; ##
		while ($phonemes[$i] eq $keyword_phonemes[$j]) {
			if ($first_occurance) {
				$first_occurance = 0;
				$first_phoneme_loc = $i;
			}
			$num_phonemes_found++; 
			#print "$i $phonemes[$i]  $keyword_phonemes[$j] $first_phoneme_loc $num_phonemes_found\n"; ##
			$j++; 
			$i++;
			last if (!exists $keyword_phonemes[$j]);
		}
		if ($num_phonemes_found == scalar(@keyword_phonemes)) {
			last;
		}
		else {
			$i-- if ($num_phonemes_found != 0);
 			$num_phonemes_found = 0;
			$first_occurance = 1;
			$j = 0;
		}
		$i++;
	}
	for ($i = 0; $i < scalar(@keyword_phonemes); $i++) {
		print  $start_times[$first_phoneme_loc + $i]." ";
		print KEYWORD_ALIGNMENT_LIST $start_times[$first_phoneme_loc + $i]." ";
	}
	if (($first_phoneme_loc + $i) == scalar(@phonemes)) {
		$end_frame = $num_frames-1;
	} else {
		$end_frame = $start_times[$first_phoneme_loc + $i]-1;
	}
	print $end_frame."\n";
	print KEYWORD_ALIGNMENT_LIST $end_frame."\n";

	for ($i = 0; $i < scalar(@keyword_phonemes); $i++) {
		print $keyword_phonemes[$i]." ";
		print KEYWORD_PHONEME_LIST $keyword_phonemes[$i]." ";
	}
	print "\n";
	print KEYWORD_PHONEME_LIST "\n";

	# find the list of phonemes in a .phoneme file

	# output the list of phonemes and its correponding list of start time and end time

}
close(TRAIN_NEG);
close(TRAIN_POS);
close(KEYWORD_PHONEME_LIST);
close(KEYWORD_ALIGNMENT_LIST);


print "Done.\n";


# fisher_yates_shuffle( \@array ) : generate a random permutation
# of @array in place
sub fisher_yates_shuffle {
	my $array = shift;
	my $i;
	for ($i = @$array; --$i; ) {
		my $j = int rand ($i+1);
		next if $i == $j;
		@$array[$i,$j] = @$array[$j,$i];
	}
}


