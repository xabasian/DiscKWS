#ifndef _CLASSIFIER_H
#define _CLASSIFIER_H

/************************************************************************
 Project:  Segmentation and Alignment
 Module:   Classifier
 Purpose:  Alignment discriminative algorithm
 Date:     10 Apr., 2004
 Programmer: Shai Shalev-Shwartz

 *************************** INCLUDE FILES ******************************/
#include "../learning_tools/infra2/infra.h"
#include <string>
#include <map>
#include "Dataset.h"
#include "../learning_tools/active_set/active_set.h"
#include "../learning_tools/active_set/active_set.imp"
#include "../learning_tools/kernels/kernels.h"
#include "../learning_tools/kernels/kernels.imp"

class Classifier
{
 public:
  Classifier(double _B, double _C, std::string kernel_name, int num_phonemes, 
	     int mfcc_dim, int num_frames);
  ~Classifier();
  double update(infra::vector_view x, int y, int y_hat);
  infra::vector_view predict(infra::vector_view x, int &y_hat);
  void load(std::string &filename);
  void save(std::string &filename);
  int num_supports() { return support.size(); }
  void averaging();
  
 protected:
  double BB;
  double CC;
  kernels::KernelsBuilder K; 
  ActiveSet alpha;
  ActiveSet support;
  infra::matrix scaled_alpha;
  infra::vector ranks;
  int instance_index;
  bool averaging_enabled;
};

#endif // _CLASSIFIER_H
