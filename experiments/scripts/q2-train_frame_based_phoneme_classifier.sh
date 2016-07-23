#!/bin/bash -fex

# phoneme frame-based classifier parameters
SIGMA=19
C=1
B=0.5

# build kernel file
echo "gaussian" > config/gaussain_$SIGMA.kernel
echo "2 1 $SIGMA" >> config/gaussain_$SIGMA.kernel

# train passive-aggressive phoneme frame-based classifier 
mkdir -p models
../bin/PhonemeFrameBasedTrain -C $C -B $B -kernel config/gaussain_$SIGMA.kernel -mfcc_stats config/mfcc.stats config/train_frame_based_classifier_1500.mfc config/train_frame_based_classifier_1500.labels config/phonemes_39 models/pa_phoeneme_frame_based.C_$C.B_$B.sigma_$SIGMA.model 

# decode the rest of the training files 
../bin/PhonemeFrameBasedDecode -kernel config/gaussain_$SIGMA.kernel -mfcc_stats config/mfcc.stats -averaging -scores config/train_frame_based_classifier_1500_rest.scores config/train_frame_based_classifier_1500_rest.mfc config/train_frame_based_classifier_1500_rest.labels config/phonemes_39 models/pa_phoeneme_frame_based.C_$C.B_$B.sigma_$SIGMA.model 

# decode the all the test files
../bin/PhonemeFrameBasedDecode -kernel config/gaussain_$SIGMA.kernel -mfcc_stats config/mfcc.stats -averaging -scores config/TEST.scores config/TEST.mfc config/TEST.labels config/phonemes_39 models/pa_phoeneme_frame_based.C_$C.B_$B.sigma_$SIGMA.model 
