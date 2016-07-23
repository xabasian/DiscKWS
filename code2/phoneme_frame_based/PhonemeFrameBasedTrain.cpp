/************************************************************************
Project:  Phoeneme Frame-Based Classification
Module:   PhonemeFrmaeBasedTrain
Purpose:  Main entry point for training
Date:     17 Jan., 2005
Programmer: Joseph Keshet

Function List:
main - Main entry point

**************************** INCLUDE FILES *****************************/
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include "../learning_tools/cmdline/cmd_line.h"
#include "Classifier.h"
#include "Dataset.h"

using namespace std;

/************************************************************************
Function:     main

Description:  Main entry point
Inputs:       int argc, char *argv[] - main input params
Output:       int - always 0.
Comments:     none.
***********************************************************************/
int main(int argc, char **argv) 
{
  // Parse command line
  string input_filelist;
  string target_filelist;
  string phonemes_filename;
  string classifier_filename;
  string mfcc_stats_filename;
  string kernel_name;
  string silence_symbol;
  double B;
  double C;
  learning::cmd_line cmdline;
  cmdline.info("Phoeneme Frame-Based Classifier Training");
  cmdline.add("-kernel", "kernel file name", &kernel_name, "");
  cmdline.add("-C", "C parameter [1.0]", &C, 1.0);
  cmdline.add("-B", "B parameter [0.5]", &B, 0.5);
  cmdline.add("-mfcc_stats", "feature statstics filename", &mfcc_stats_filename, "");
  cmdline.add("-silence", "silence phoneme symbol [sil]", &silence_symbol, "sil");
  cmdline.add_master_option("input-filelist", &input_filelist);
  cmdline.add_master_option("target-filelist", &target_filelist);
  cmdline.add_master_option("phonemes", &phonemes_filename);
  cmdline.add_master_option("classifier", &classifier_filename);
  //************
  argv[0] = "test";
  argv[1] = "-kernel";
  argv[2] = "gaussian_19.kernel";
  argv[3] = "-C";
  argv[4] = "1";
  argv[5] = "-B";
  argv[6] = "0.5";
  argv[7] = "-mfcc_stats";
  //argv[8] = "timit/mfcc_stats_me.out";


  //argv[9] = "timit/listFiles/timit_train_frame_based.200.list";
  //argv[10] = "timit/listFiles/timit_train_frame_based.200.labels";
  //argv[11] = "config/phoneme_map_timit61_to_leehon39";
  //argv[12] = "timit/classifier.out";
  argc = 13;
  //************
  int rc = cmdline.parse(argc, argv);
  if (rc < 4) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // Set the silence symbol
  LabelType::set_silence_symbol(silence_symbol);
  
  // Load set of phonemes
  LabelType::load_phoneme_map(phonemes_filename);

  // load mfcc statistics (mean and std)
  InstanceType::load_mfcc_stats(mfcc_stats_filename);
  
  // begining of the training set
  infra::matrix X;
  IntVector Y;
  Dataset training_dataset(input_filelist, target_filelist);
  cout << "Loading dataset... " << flush;
  training_dataset.read_all(X, Y);
  int num_frames = Y.size();
  cout << "Read " << num_frames << " frames." << endl;
  
  // random shuffle training set
  cout << "Random shuffle all frames in the training set... " << flush;
  int frame_numbers[num_frames];
  for (int j=0; j < num_frames; j++)
    frame_numbers[j] = j;
  std::random_shuffle(frame_numbers, frame_numbers+Y.size());
  cout << "Done." << endl;

  // Initiate classifier  
  Classifier classifier(B, C, kernel_name, LabelType::num_phonemes, InstanceType::mfcc_dim, num_frames); 

  uint cumulative_error = 0;
  uint cumulative_num_frames = 0;
  double cumulative_loss = 0;

  // Run over all dataset
   for (int l=0; l < num_frames; l++) {
     
     int i = frame_numbers[l];
     infra::vector_view x = X.row(i);
     int y = Y[i];
     int y_hat;
     
     // don't count unwanted frames in the learning process
     if (y < 0) continue;
     
     // predict label 
     classifier.predict(x, y_hat);    
     if (y != y_hat) cumulative_error++;
     cumulative_num_frames++;
     double loss = classifier.update(x, y, y_hat);
     cumulative_loss += loss;
   }
   
   cout << " Comulative error=" << cumulative_error/double(cumulative_num_frames)
     << " loss=" << cumulative_loss/double(cumulative_num_frames) 
     << " supports=" << classifier.num_supports() << endl;

   // save classifier 
   cout << "Saving classifier to disk... " << flush;
   classifier.save(classifier_filename);
   cout << "Done." << endl;  
  
   return EXIT_SUCCESS;
}



// ------------------------------- EOF -----------------------------//
