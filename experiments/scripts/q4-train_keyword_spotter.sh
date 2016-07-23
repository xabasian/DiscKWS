#!/bin/bash -fe

# Here is how the keywords for train were picked up:
 tail -n 900 config/timit_word_statistics.txt  | \
 	awk '{print $1}' | \
  	scripts/random_file_lines.pl - | \
  	head -n 200 > config/train_keyword_spotting.keywords
sed "s/.mfc$//" config/train_keyword_spotting.mfc > config/train_keyword_spotting.files
scripts/timit_keyword_for_train.pl \
	config/train_keyword_spotting.keywords \
	config/train_keyword_spotting.files \
	config/train_keyword_spotting.pos_files \
	config/train_keyword_spotting.neg_files \
	config/train_keyword_spotting.phonemes \
	config/train_keyword_spotting.alignment

# Here is how the keywords for validation were picked up:
 tail -n 900 config/timit_word_statistics.txt  | \
  	awk '{print $1}' | \
  	scripts/random_file_lines.pl - | \
  	head -n 400 | tail -n 200 > config/train_val_keyword_spotting.keywords

cat config/train_keyword_spotting.pos_files config/train_keyword_spotting.neg_files | sed "s/.mfc$//" > config/train_keyword_spotting.file_list

scripts/elements_in_one_array_but_not_another.pl config/train_keyword_spotting.files config/train_keyword_spotting.file_list >  config/train_val_keyword_spotting.file_list

scripts/timit_keyword_for_train.pl \
	config/train_val_keyword_spotting.keywords \
	config/train_val_keyword_spotting.file_list \
	config/train_val_keyword_spotting.pos_files \
	config/train_val_keyword_spotting.neg_files \
	config/train_val_keyword_spotting.phonemes \
	config/train_val_keyword_spotting.alignment

# now train

# parameters (the same as forced aligner)
beta1=0.01
beta2=1.0
beta3=1.0

# start keyword spotting from scratch:
# echo "7 0 0 0 0 0 0 0" > models/disc_keyword_spotter
# echo "" >>  models/disc_keyword_spotter

# or, start using forced alignment bootstrap
cp models/forced_alignment.beta1_$beta1.beta2_$beta2.beta3_$beta3.gamma_1.0.model models/disc_keyword_spotter.beta1_$beta1.beta2_$beta2.beta3_$beta3.model

../bin/KeywordSpotterTrain \
-beta1 $beta1 \
-beta2 $beta2 \
-beta3 $beta3 \
-val_pos_filelist config/train_val_keyword_spotting.pos_files \
-val_neg_filelist config/train_val_keyword_spotting.neg_files \
-val_keyword_phoneme_list config/train_val_keyword_spotting.phonemes \
-val_keyword_alignment_list config/train_val_keyword_spotting.alignment \
config/train_keyword_spotting.pos_files \
config/train_keyword_spotting.neg_files \
config/train_keyword_spotting.phonemes \
config/train_keyword_spotting.alignment \
config/phonemes_39 \
config/phonemes_39.stats \
models/disc_keyword_spotter.beta1_$beta1.beta2_$beta2.beta3_$beta3.model
