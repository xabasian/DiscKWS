#!/bin/bash -fe

# beta1=$1
# beta2=$2
# beta3=$3
# min_sqrt_gamma=$4

beta1=0.01
beta2=1.0
beta3=1.0
min_sqrt_gamma=1.0

exp_name=forced_alignment.beta1_$beta1.beta2_$beta2.beta3_$beta3.gamma_$min_sqrt_gamma

for SET in train_val_forced_alignment_100 train_forced_alignment_150 test_core ; do
sed "s/labels$/scores/" config/$SET.labels > config/$SET.scores
sed "s/labels$/dist/" config/$SET.labels > config/$SET.dist
sed "s/labels$/phonemes/" config/$SET.labels > config/$SET.phonemes
sed "s/labels$/start_times/" config/$SET.labels > config/$SET.start_times
done
echo "Training forced-alignment: $exp_name"
../bin/ForcedAlignmentTrain \
-remove_silence \
-val_scores_filelist config/train_val_forced_alignment_100.scores \
-val_dists_filelist config/train_val_forced_alignment_100.dist \
-val_phonemes_filelist config/train_val_forced_alignment_100.phonemes \
-val_start_times_filelist config/train_val_forced_alignment_100.start_times \
-beta1 $beta1 \
-beta2 $beta2 \
-beta3 $beta3 \
-min_gamma $min_sqrt_gamma \
config/train_forced_alignment_150.scores \
config/train_forced_alignment_150.dist \
config/train_forced_alignment_150.phonemes \
config/train_forced_alignment_150.start_times \
config/phonemes_39 \
config/phonemes_39.stats \
models/$exp_name.model 

echo "Decoding forced-alignment: $exp_name"
../bin/ForcedAlignmentDecode \
-remove_silence \
-beta1 $beta1 \
-beta2 $beta2 \
-beta3 $beta3 \
config/test_core.scores \
config/test_core.dist \
config/test_core.phonemes \
config/test_core.start_times \
config/phonemes_39 \
config/phonemes_39.stats \
models/$exp_name.model

echo "Force alignment completed successfully"
