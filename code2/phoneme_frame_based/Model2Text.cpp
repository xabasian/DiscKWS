/************************************************************************
Project:  Phoeneme Frame-Based Classification
Module:   Model2Text
Purpose:  Main entry point for training
Date:     29 Dec., 2006
Programmer: Joseph Keshet

**************************** INCLUDE FILES *****************************/
#include <iostream>
#include <fstream>
#include "../learning_tools/cmdline/cmd_line.h"
#include "../learning_tools/active_set/active_set.h"
#include "../learning_tools/active_set/active_set.imp"

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
  string model_file;
  string support_file;
  string alpha_file;
  learning::cmd_line cmdline;
  cmdline.info("Convert passive-aggressive model to text");
  cmdline.add_master_option("<model (input)>", &model_file);
  cmdline.add_master_option("<supports (output)>", &support_file);
  cmdline.add_master_option("<alpha (output)>", &alpha_file);
  int rc = cmdline.parse(argc, argv);
  if (rc < 3) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  FILE *pfile;
  pfile = fopen(model_file.c_str(), "rb");
  if (pfile == NULL) {
    cerr << "Error: Unable to open " << model_file << " for reading. Aborting..." << endl;
    exit(-1);
  }
  ActiveSet alpha(pfile);
  ActiveSet support(pfile);
  fclose(pfile);

  ofstream ofs_support(support_file.c_str());
  ofs_support << support() << endl;
  ofs_support.close();

  ofstream ofs_alpha(alpha_file.c_str());
  ofs_alpha << alpha() << endl;
  ofs_alpha.close();

  return EXIT_SUCCESS;
}
