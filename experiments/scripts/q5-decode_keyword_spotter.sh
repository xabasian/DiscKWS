#!/bin/bash -fe

keyword_list=config/timit_test_set.keywords

# model parameters (should be the same as training)
beta1=0.01
beta2=1.0
beta3=1.0

# # Uncomment these lines to work on the model trained with the forced alignment
# mv models/disc_keyword_spotter.beta1_$beta1.beta2_$beta2.beta3_$beta3.model models/disc_keyword_spotter.beta1_$beta1.beta2_$beta2.beta3_$beta3.model.backup
# cp models/forced_alignment.beta1_$beta1.beta2_$beta2.beta3_$beta3.gamma_1.0.model models/disc_keyword_spotter.beta1_$beta1.beta2_$beta2.beta3_$beta3.model

for k in `cat $keyword_list`
do
	echo "*** keyword: $k ***"
	echo "*** positive files decoding ***" 
	../bin/KeywordSpotterDecode \
	-beta1 $beta1 \
	-beta2 $beta2 \
	-beta3 $beta3 \
	keywords/$k.pos_files \
	keywords/$k.phonemes \
	config/phonemes_39 \
	config/phonemes_39.stats \
	models/disc_keyword_spotter.beta1_$beta1.beta2_$beta2.beta3_$beta3.model | tee keywords/$k.pos_decode
	grep confidence keywords/$k.pos_decode | cut -d' ' -f 2 > keywords/$k.pos_conf
	echo "*** negative files decoding ***"
	../bin/KeywordSpotterDecode \
	-beta1 $beta1 \
	-beta2 $beta2 \
	-beta3 $beta3 \
	keywords/$k.neg_files \
	keywords/$k.phonemes \
	config/phonemes_39 \
	config/phonemes_39.stats \
	models/disc_keyword_spotter.beta1_$beta1.beta2_$beta2.beta3_$beta3.model | tee keywords/$k.neg_decode
	grep confidence keywords/$k.neg_decode | cut -d' ' -f 2 > keywords/$k.neg_conf
done
read -p #######################1
echo
rm -f disc_keyword_spotting_performance.txt
echo "Saving results to disc_keyword_spotting_performance.txt."
for k in `cat $keyword_list`
do
    auc_disc=`scripts/eval_auc_from_scores.py keywords/$k.pos_conf keywords/$k.neg_conf`
    echo $k" "DISC_KEYWORD_SPOTTING" "$auc_disc >> disc_keyword_spotting_performance.txt
done
echo
awk '{sum+=$3} END { print "Average AUC= ",sum/NR}' disc_keyword_spotting_performance.txt 