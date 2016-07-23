#ifndef _CLASSIFIER_H
#define _CLASSIFIER_H

/************************************************************************
 Project:  Segmentation and Alignment
 Module:   Classifier
 Purpose:  Alignment discriminative algorithm
 Date:     10 Apr., 2004
 Programmer: Joseph Keshet
 
 *************************** INCLUDE FILES ******************************/
#include <../learning_tools/infra2/infra.h>
#include "Dataset.h"

class Classifier
  {
  public:
    Classifier(unsigned int _frame_rate, double _min_phoneme_length, 
               double _max_phoneme_length, double _PA1_C, 
               double _beta1, double _beta2, double _beta3);
    ~Classifier();
    void load(std::string &filename);
    void save(std::string &filename);
    void load_phoneme_stats(std::string &filename);
    bool was_changed() { return (w_changed); }
    double update(PhonemeSequence& keyword, 
                  SpeechUtterance& x_p, StartTimeSequence &s_p, int end_frame_p,
                  SpeechUtterance& x_n, StartTimeSequence &s_n, int end_frame_n);
    double align_keyword(SpeechUtterance& x, PhonemeSequence& keyword, 
                         StartTimeSequence &y_hat_best, int &end_frame_best);
    double confidence_keyword(SpeechUtterance& x, PhonemeSequence& keyword, 
                              StartTimeSequence &y_hat, int end_frame);
    infra::vector_view phi(SpeechUtterance& x, PhonemeSequence& keyword, 
                           StartTimeSequence& y, int end_frame);
    infra::vector_view phi_1(SpeechUtterance& x, PhonemeSequence& keyword, int i, int t, int l);
    double phi_2(SpeechUtterance& x, PhonemeSequence& keyword, int i, int t, int l1, int l2);
    static double gamma(const StartTimeSequence &y, const StartTimeSequence &y_hat);
    static double gaussian(const double x, const double mean, const double std);
    
    //protected:
    infra::vector phoneme_length_mean;
    infra::vector phoneme_length_std;
    static int phi_size;
    unsigned int frame_rate;
    int min_num_frames;
    int max_num_frames;
    infra::vector w;
    bool w_changed;
    double PA1_C;
    double beta1;
    double beta2;
    double beta3;
  };

#endif // _CLASSIFIER_H
