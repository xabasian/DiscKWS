#!/bin/bash 

./scripts/q1-extract_features_and_labels.sh > q1.log

./scripts/q2-train_frame_based_phoneme_classifier.sh > q2.log

./scripts/q3-train_forced_alignment.sh > q3.log

./scripts/q4-train_keyword_spotter.sh > q4.log

./scripts/q5-decode_keyword_spotter.sh > Factory10db_KernelBased_RDKWS.log

