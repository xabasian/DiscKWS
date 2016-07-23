/************************************************************************
Project:  Phoneme sequence classification
Module:   Classifier
Purpose:  Alignment discriminative algorithm
Date:     10 Apr., 2004
Programmer: Joseph Keshet


**************************** INCLUDE FILES *****************************/
#include "Classifier.h"
#include <iostream>
#include <fstream>
#include <vector>

using std::vector;
using std::cout;
using std::cerr;
using std::endl;

#define _max(x,y) ( (x)>(y) ? (x) : (y) )
#define _min(x,y) ( (x)<(y) ? (x) : (y) )


/************************************************************************
Function:     Classifier::Classifier

Description:  Constructor
Inputs:       none.
Output:       none.
Comments:     none.
***********************************************************************/
Classifier::Classifier(double _BB, double _CC, std::string kernel_name, 
		       int num_phonemes, int mfcc_dim, int num_frames): 
  BB(_BB), CC(_CC), K(kernel_name), 
  alpha(num_frames, num_phonemes), 
  support(num_frames, mfcc_dim)
{
  instance_index = 0;
  ranks.resize(num_phonemes);
  averaging_enabled = false;
} 



/************************************************************************
Function:     Classifier::~Classifier

Description:  Destructor
Inputs:       none.
Output:       none.
Comments:     none.
***********************************************************************/
Classifier::~Classifier()
{
}


/************************************************************************
Function:     Classifier::predict

Description:  Predict label of instance x
Inputs:       InstanceType &x 
              LabelType &y_hat
Output:       void
Comments:     none.
***********************************************************************/
infra::vector_view Classifier::predict(infra::vector_view x, int &y_hat)
{
  if (averaging_enabled)
    ranks = K( support(), x ) * scaled_alpha;
  else
    ranks = K( support(), x ) * alpha();
  y_hat =  ranks.argmax();
  return ranks;
}


/************************************************************************
Function:     Classifier::update

Description:  Train classifier with one example 
Inputs:       infra::vector& x - example instance 
int y - label 
Output:       double - squared loss
Comments:     Assume the use of predict() before the call to this 
              function, so the ranks vector is up-to-date.
***********************************************************************/
double Classifier::update( infra::vector_view x, int y, int y_hat)
{
  double loss = 0.0;

  if (y_hat == y) {
    double tmp = ranks(y);
    ranks(y) = -99999999.9;
    y_hat = ranks.argmax();
    ranks(y) = tmp;
  }

  double margin = ranks(y) - ranks(y_hat);
  if (margin <= BB) {
    loss = 1.0 - margin;
    double tau = _min(CC , loss / (2.0*K(x,x)) );
    //double tau = _min(CC , loss / (2.0)); 
    infra::vector alpha_value(LabelType::num_phonemes);
    alpha_value.zeros();
    alpha_value(y) = tau; alpha_value(y_hat) = -tau;
    support.insert(x, instance_index);    
    alpha.insert(alpha_value, instance_index);
  }
  instance_index++;
  return loss;
}


/************************************************************************
Function:     Classifier::averaging

Description:  Prepare alpha for averaging
Inputs:       none.
Output:       none.
Comments:     none.
***********************************************************************/
void Classifier::averaging(void)
{
  scaled_alpha.resize(alpha().height(), alpha().width());
  scaled_alpha = alpha();
  for (uint i=0; i < scaled_alpha.height(); i++) {    
    double scaling = (alpha.num_examples()-alpha.get_logical_index(i))/double(alpha.num_examples());
    for (uint j=0; j < scaled_alpha.width(); j++) {
      scaled_alpha(i,j) *= scaling;
    }
  }
  averaging_enabled = true;
}

/************************************************************************
Function:     Classifier::load

Description:  Loads a classifier 
Inputs:       string & filename
Output:       none.
Comments:     none.
***********************************************************************/
void Classifier::load(std::string &filename)
{
  FILE *pfile;
  pfile = fopen(filename.c_str(), "rb");
  if (pfile == NULL) {
    cerr << "Error: Unable to open " << filename << " for reading. Aborting..." << endl;
    exit(-1);
  }
  alpha.load_binary(pfile);
  support.load_binary(pfile);
  fclose(pfile);
}


/************************************************************************
Function:     Classifier::save

Description:  Saves a classifier 
Inputs:       string & filename
Output:       none.
Comments:     none.
***********************************************************************/
void Classifier::save(std::string &filename)
{
  FILE *pfile;
  pfile = fopen(filename.c_str(), "wb");
  if (pfile == NULL) {
    cerr << "Error: Unable to open " << filename << " for writing. Aborting..." << endl;
    exit(-1);
  }
  alpha.save_binary(pfile);
  support.save_binary(pfile);
  fclose(pfile);
}


// --------------------- EOF ------------------------------------//

