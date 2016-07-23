/************************************************************************
 Copyright (c) 2006 Joseph Keshet
 
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without 
 modification, are permitted provided that the following conditions are 
 met: Redistributions of source code must retain the above copyright 
 notice, this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the 
 documentation and/or other materials provided with the distribution.
 The name of the author may not be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, 
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 THE POSSIBILITY OF SUCH DAMAGE.
 ************************************************************************/

/************************************************************************
 Project:  Discriminative Keyword Spotting
 Module:   KeywordSpotterTrain
 Purpose:  Main entry point
 Date:     7 Aug., 2006
 Programmer: Joseph Keshet
 
 Function List:
 main - Main entry point
 
 **************************** INCLUDE FILES *****************************/
#include <iostream>
#include <fstream>
#include <../learning_tools/cmdline/cmd_line.h>
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
  unsigned int frame_rate = 10;
  double min_phoneme_length, max_phoneme_length;
  string silence_symbol;
  string filename_list;
  string keyword_phoneme_list; 
  string keyword_alignment_list; 
  string phonemes_filename;
  string phoneme_stats_filename;
  string dist_stats_filename;
  string classifier_filename;
  double C = 0.0;
  double beta1, beta2, beta3;
  bool remove_silence;
  string scores_ext;
  
  learning::cmd_line cmdline;
  cmdline.info("Decoding discriminative Keyword Spotter");
  cmdline.add("-min_phoneme_length", "min. phoneme duration in msec [20]", &min_phoneme_length, 20);
  cmdline.add("-max_phoneme_length", "max. phoneme duration in msec [330]", &max_phoneme_length, 330);
  cmdline.add("-silence_symbol", "silence symbol [sil]", &silence_symbol, "sil");
  cmdline.add("-dist_stats", "feature statstics filename [dist_stats.out]", 
              &dist_stats_filename, "dist_stats.out");
  cmdline.add("-remove_silence", "remove pre/post silence from data", &remove_silence, false);  
  cmdline.add("-beta1", "weight of the distance feature", &beta1, 0.01);
  cmdline.add("-beta2", "weight of the duration feature", &beta2, 1.0);
  cmdline.add("-beta3", "weight of the speaking rate feature", &beta3, 1.0);
  cmdline.add("-scores_ext", "fli extension for the scores", &scores_ext, ".scores");
  cmdline.add_master_option("filelist", &filename_list);
  cmdline.add_master_option("keyword_phoneme_list", &keyword_phoneme_list);
  cmdline.add_master_option("phonemes", &phonemes_filename);	
  cmdline.add_master_option("phoneme-stats", &phoneme_stats_filename);
  cmdline.add_master_option("classifier", &classifier_filename);
  int rc = cmdline.parse(argc, argv);
  if (rc < 5) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // phoneme symbol to number mapping (Lee and Hon, 89)
  PhonemeSequence::load_phoneme_map(phonemes_filename, silence_symbol);
  
  // Initiate classifier
  Classifier classifier(frame_rate, min_phoneme_length, max_phoneme_length, C, beta1, beta2, beta3);
  classifier.load(classifier_filename);
  classifier.load_phoneme_stats(phoneme_stats_filename);
  
  Dataset dataset(filename_list, keyword_phoneme_list);
  
  // Run over all dataset
  for (uint l=0; l <  dataset.size(); l++) {
    
    SpeechUtterance x;
    StartTimeSequence s;
    int end_frame;
    StartTimeSequence true_s;
    int true_end_frame;
    PhonemeSequence keyword;
    double confidence;
    
    cout << "========================================================================" << endl;
    
    // read next example for dataset
    int frame_offset = dataset.read(x, keyword, true_s, true_end_frame, scores_ext, remove_silence);
    // predict
    try {
      confidence = classifier.align_keyword(x, keyword, s, end_frame);
      int abs_end_frame = end_frame + frame_offset;
      StartTimeSequence abs_s(s);
      for (int i = 0; i < int(abs_s.size()); i++) 
        abs_s[i] = s[i] + frame_offset;
      cout << "keyword[" << l << "]=/" << keyword << "/" << std::endl;
      cout << "alignment[" << l << "]=" << abs_s << " " << abs_end_frame << endl;
      cout << "confidence[" << l << "]= " << confidence << endl;			
      cout << "phi=" << classifier.phi(x,keyword, s, end_frame) << std::endl;
      
      if (0) {
        int start_phoneme = 6;
        StartTimeSequence true_s2; true_s2.resize(keyword.size());
        for (uint i=0; i < keyword.size(); i++) {
          true_s2[i] = true_s[i+start_phoneme]-true_s[1];
        }
        int true_end_frame2 = true_s[start_phoneme+keyword.size()]-1-true_s[1];
        cout << "alignment[" << l << "]=" << true_s2 << " " << true_end_frame2 << endl;
        confidence = classifier.confidence_keyword(x, keyword, true_s2, true_end_frame2);
        cout << "true confidence[" << l << "]= " << confidence << endl;
      }
    }
    catch (std::bad_alloc&) {
      std::cerr << "Error: cannot allocate memory. Skipping..." << std::endl;
    }
  } 
  
  cout << "Done." << endl;  
  
  return EXIT_SUCCESS;
}

// ------------------------------- EOF -----------------------------//
