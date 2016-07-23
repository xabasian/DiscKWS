/************************************************************************
 Project:  Phoneme Alignment
 Module:   Dataset Definition
 Purpose:  Defines the data structs of the instances and the labels
 Date:     25 Jan., 2005
 
 Function List:
 SpeechUtterance
 read - Read Instance from file stream
 
 StartTimeSequence
 read - Read Label from file stream
 
 **************************** INCLUDE FILES *****************************/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "Dataset.h"

#define NORM_SCORES_0_1

// PhonemeSequence static memebers definitions
unsigned int PhonemeSequence::num_phonemes;
std::map<std::string, int> PhonemeSequence::phoneme2index;
std::map<int,std::string> PhonemeSequence::index2phoneme;
infra::vector SpeechUtterance::dist_mean((unsigned long)0);
infra::vector SpeechUtterance::dist_std((unsigned long)0);

/************************************************************************
 Function:     SpeechUtterance::read
 
 Description:  Read SpeechUtterance from file stream
 Inputs:       string &filename
 Output:       bool - true if success, otherwise false
 Comments:     none.
 ***********************************************************************/
void SpeechUtterance::read(std::string &scores_filename, 
                           std::string &dists_filename)
{
  // load score matrix
  std::ifstream ifs_scores(scores_filename.c_str());
  if (ifs_scores.good()) {
    infra::matrix tmp(ifs_scores);
    scores.resize(tmp.height(), tmp.width());
#ifdef NORM_SCORES_0_1
    // normalize scores to be in [0,1]
    for (uint i = 0; i < tmp.height(); i++) {
      double max_scores_i = tmp.row(i).max();
      double min_scores_i = tmp.row(i).min();
      for (uint j = 0; j < tmp.width(); j++) {
        scores(i,j) = (tmp(i,j)-min_scores_i)/(max_scores_i-min_scores_i);
      }
    }
#else
    scores = tmp;
#endif
  }
  else {
    std::cerr << "Error: Unable to read scores from " << scores_filename 
    << std::endl;
    exit(-1);
  } 
  ifs_scores.close();
  
  // load dist matrix
  std::ifstream ifs_distances(dists_filename.c_str());
  if (ifs_distances.good()) {
    infra::matrix tmp(ifs_distances);
    distances.resize(tmp.height(), tmp.width());
    distances = tmp;
  }
  else {
    std::cerr << "Error: Unable to read distances from " << dists_filename << std::endl;
    exit(-1);
  } 
  ifs_distances.close();
  
#if 0	
  // load dist matrix
  std::ifstream ifs_distances(dists_filename.c_str());
  if (ifs_distances.good()) {
    infra::matrix tmp(ifs_distances);
    distances.resize(tmp.height(), tmp.width());
    //distances = tmp;
    for (int i=0; i < int(tmp.height()); i++)
      for (int j=0; j < int(tmp.width()); j++)
        distances(i,j) = (tmp(i,j) - dist_mean(j))/dist_std(j);
  }
  else {
    std::cerr << "Error: Unable to read distances from " << dists_filename 
    << std::endl;
    exit(-1);
  } 
  ifs_distances.close();
#endif
}

/************************************************************************
 Function:     SpeechUtterance::size()
 
 Description:  Size of features phi
 Inputs:       none.
 Output:       void.
 Comments:     none.
 ***********************************************************************/
unsigned long SpeechUtterance::size() 
{ 
  return ( scores.width() + distances.width() + 1 ); 
} 

/************************************************************************
 Function:     SpeechUtterance::load_dist_stats
 
 Description:  Read mfcc statistics from an external file
 Inputs:       string &filename
 Output:       void.
 Comments:     none.
 ***********************************************************************/
void SpeechUtterance::load_dist_stats(std::string &filename)
{
  std::ifstream ifs(filename.c_str());
  if (!ifs.good()) {
    std::cerr << "Warning: Unable to load dist stats from " << filename << std::endl;
    dist_mean.resize(4);
    dist_mean.zeros();
    dist_std.resize(4);
    dist_std.ones();
  }
  else {
    infra::matrix tmp1(ifs);
    dist_mean.resize(tmp1.width());
    dist_mean = tmp1.row(0);
    dist_std.resize(tmp1.width());
    dist_std = tmp1.row(1);
    std::cout << "Info: load feature statistics from " << filename << std::endl;
  }
  ifs.close();
  std::cout << "dist_mean=" << dist_mean << std::endl;
  std::cout << "dist_std=" << dist_std << std::endl;
}


/************************************************************************
 Function:     PhonemeSequence::load_phoneme_map
 
 Description:  Load the phonemes file andbuild maps
 Inputs:       string &filename
 Output:       void.
 Comments:     none.
 ***********************************************************************/
void PhonemeSequence::load_phoneme_map(std::string &phonemes_filename, 
                                       std::string &silence_symbol)
{
  // Generate phoneme mapping
  std::ifstream phonemes_ifs(phonemes_filename.c_str());
  if (!phonemes_ifs.good()) {
    std::cerr << "Error: Unable to open phonemes file " << phonemes_filename << std::endl;
    exit(-1);
  }
  int index = 0;
  bool found_silence_symbol = false;
  while (phonemes_ifs.good()) {
    std::string phoneme;    
    phonemes_ifs >> phoneme;
    if (phoneme == "") continue;
    if (phoneme == silence_symbol) found_silence_symbol = true;
    index2phoneme[index] = phoneme;
    phoneme2index[phoneme] = index;
    index++;
  }
  phonemes_ifs.close();
  if (!found_silence_symbol) {
    std::cerr << "Error: didn't find the silence symbol \"" << silence_symbol 
    << "\" inside phonemes file: " << phonemes_filename << std::endl;
    exit(-1);
  }
  
  num_phonemes = index;
  
  //	for (uint i = 0; i < num_phonemes; i++)
  //		std::cout << i << " " << index2phoneme[i] << std::endl;
}

/************************************************************************
 Function:     PhonemeSequence::read
 
 Description:  Read PhonemeSequence from file stream
 Inputs:       string &filename
 Output:       void.
 Comments:     none.
 ***********************************************************************/
void PhonemeSequence::read(std::string &filename)
{
  std::ifstream ifs(filename.c_str());
  if (!ifs.good()) {
    std::cerr << "Error: Unable to open phonemes file " << filename << std::endl;
    exit(-1);
  }
  while (ifs.good()) {
    std::string phoneme;
    ifs >> phoneme;
    if (phoneme == "") break;
    if (phoneme == "del") phoneme = "t";
    push_back(PhonemeSequence::phoneme2index[phoneme]);
  }
  ifs.close();
}

/************************************************************************
 Function:     operator << for PhonemeSequence
 
 Description:  Write PhonemeSequence& vector to output stream
 Inputs:       std::ostream&, const StringVector&
 Output:       std::ostream&
 Comments:     none.
 ***********************************************************************/
std::ostream& operator<< (std::ostream& os, const PhonemeSequence& y)
{
  for (uint i=0; i < y.size(); i++)
    os << PhonemeSequence::index2phoneme[ y[i] ] << " ";
  
  return os;
}

/************************************************************************
 Function:     operator << for PhonemeSequence
 
 Description:  Write PhonemeSequence& vector to output stream
 Inputs:       std::ostream&, const StringVector&
 Output:       std::ostream&
 Comments:     none.
 ***********************************************************************/
std::ostream& operator<< (std::ostream& os, const StartTimeSequence& y)
{
  for (uint i=0; i < y.size(); i++)
    os <<  y[i]  << " ";
  
  return os;
}

/************************************************************************
 Function:     Dataset::Dataset
 
 Description:  Constructor
 Inputs:       std::string dataset_filename
 Output:       void.
 Comments:     none.
 ***********************************************************************/
Dataset::Dataset(std::string& pos_filename, std::string& neg_filename, 
                 std::string& keyword_phoneme_filename, 
                 std::string& keyword_alignment_filename)
{
  // Read list of files into StringVector
  pos_file_list.read(pos_filename);
  neg_file_list.read(neg_filename);
  keyword_phoneme_list.read(keyword_phoneme_filename);
  keyword_alignment_list.read(keyword_alignment_filename);  
  current_file = 0;
}

/************************************************************************
 Function:     Dataset::Dataset
 
 Description:  Constructor
 Inputs:       std::string dataset_filename
 Output:       void.
 Comments:     none.
 ***********************************************************************/
Dataset::Dataset(std::string& neg_filename,
                 std::string& keyword_phoneme_filename)
{
  // Read list of files into StringVector
  neg_file_list.read(neg_filename);
  keyword_phoneme_list.read(keyword_phoneme_filename);
  if (neg_file_list.size() != keyword_phoneme_list.size()) {
    std::string keyword_phonemes = keyword_phoneme_list[0];
    std::cerr << "Info: all files are checked against the same keyword /"
    << keyword_phonemes << "/" << std::endl;
    keyword_phoneme_list.resize(neg_file_list.size());
    for (int i=0; i < int(keyword_phoneme_list.size()); i++)
      keyword_phoneme_list[i] = keyword_phonemes;
  }
  current_file = 0;
}


/************************************************************************
 Function:     Dataset::read
 
 Description:  Read next instance and label
 Inputs:       SpeechUtterance&
 StartTimeSequence&
 Output:       void.
 Comments:     none.
 ***********************************************************************/
void Dataset::read(SpeechUtterance &x_p, SpeechUtterance &x_n, 
                   PhonemeSequence &keyword, StartTimeSequence &s, 
                   int &end_frame, std::string scores_ext, bool remove_silence)
{
  std::cout << "current file=" << current_file 
  << " pos:" << pos_file_list[current_file] 
  << " neg:" << neg_file_list[current_file] << std::endl;
  
  std::string string1 = pos_file_list[current_file]+scores_ext;
  std::string string2 = pos_file_list[current_file]+".dist";
  x_p.read(string1, string2);
  string1 = neg_file_list[current_file]+scores_ext;
  string2 = neg_file_list[current_file]+".dist";
  x_n.read(string1, string2);
  string1 = pos_file_list[current_file]+".start_times";
  string2 = neg_file_list[current_file]+".start_times";
  StartTimeSequence s_p;
  s_p.read(string1);
  StartTimeSequence s_n;
  s_n.read(string2);
  
  std::istringstream keyword_phoneme_string(keyword_phoneme_list[current_file]);
  std::string keyword_phoneme;
  keyword.clear();
  while (keyword_phoneme_string >> keyword_phoneme) {
    if (keyword_phoneme == "del") keyword_phoneme = "t";
    int index = PhonemeSequence::phoneme2index[keyword_phoneme];
    std::string keyword_phoneme_check =  PhonemeSequence::index2phoneme[index];
    if (keyword_phoneme != keyword_phoneme_check)
      std::cerr << "Error: /" << keyword_phoneme << "/ is not a legal phoneme" << std::endl;
    keyword.push_back(index);
  }
  
  std::istringstream keyword_alignment_string(keyword_alignment_list[current_file]);
  std::string a_start_time;
  s.clear();
  while (keyword_alignment_string >> a_start_time) {
    if (s.size() == keyword.size()) 
      end_frame = int(atoi(a_start_time.c_str()));
    else
      s.push_back(int(atoi(a_start_time.c_str())));
  }
  
  if (remove_silence) {
    ///////////////////////////
    // the code below is used to remove the leading silence at the 
    // begining of each utterance and the following silnce at the end
    infra::matrix tmp1(x_p.scores);
    x_p.scores.resize(s_p[s_p.size()-1]-s_p[1],tmp1.width());
    x_p.scores = tmp1.submatrix(s_p[1],0,x_p.scores.height(), x_p.scores.width());
    
    infra::matrix tmp2(x_p.distances);
    x_p.distances.resize(s_p[s_p.size()-1]-s_p[1],tmp2.width());
    x_p.distances = tmp2.submatrix(s_p[1],0,x_p.distances.height(), x_p.distances.width());
    
    for (uint i=0;i<s.size();++i)
      s[i] -= s_p[1];
    end_frame -= s_p[1];
    
    infra::matrix tmp3(x_n.scores);
    x_n.scores.resize(s_n[s_n.size()-1]-s_n[1],tmp3.width());
    x_n.scores = tmp3.submatrix(s_n[1],0,x_n.scores.height(), x_n.scores.width());
    
    infra::matrix tmp4(x_n.distances);
    x_n.distances.resize(s_n[s_n.size()-1]-s_n[1],tmp4.width());
    x_n.distances = tmp4.submatrix(s_n[1],0,x_n.distances.height(), x_n.distances.width());
    ///////////////////////////
  }
  current_file++;
  
  if (end_frame >= int(x_p.scores.height()))
    end_frame = x_p.scores.height()-1;
}

/************************************************************************
 Function:     Dataset::read
 
 Description:  Read next instance and label
 Inputs:       SpeechUtterance&
 StartTimeSequence&
 Output:       void.
 Comments:     none.
 ***********************************************************************/
int Dataset::read(SpeechUtterance &x_n, PhonemeSequence &keyword, StartTimeSequence &s, 
                  int &end_frame, std::string scores_ext, bool remove_silence)
{
  std::cout << "current file=" << current_file 
  << " " << neg_file_list[current_file] << std::endl;
  
  std::string string1;
  std::string string2;
  string1 = neg_file_list[current_file]+scores_ext;
  string2 = neg_file_list[current_file]+".dist";
  x_n.read(string1, string2);
  int n = 0;
  //  StartTimeSequence s;
  string2 = neg_file_list[current_file]+".start_times";
  n = s.read(string2);
  
  std::istringstream keyword_phoneme_string(keyword_phoneme_list[current_file]);
  std::string keyword_phoneme;
  keyword.clear();
  while (keyword_phoneme_string >> keyword_phoneme) {
    if (keyword_phoneme == "del") keyword_phoneme = "t";
    int index = PhonemeSequence::phoneme2index[keyword_phoneme];
    std::string keyword_phoneme_check =  PhonemeSequence::index2phoneme[index];
    if (keyword_phoneme != keyword_phoneme_check)
      std::cerr << "Error: /" << keyword_phoneme << "/ is not a legal phoneme" << std::endl;
    keyword.push_back(index);
  }
  int frame_offset = 0;
  if (remove_silence) {
    frame_offset = s[1];
    ///////////////////////////
    // the code below is used to remove the leading silence at the 
    // begining of each utterance and the following silnce at the end
    infra::matrix tmp1(x_n.scores);
    x_n.scores.resize(s[s.size()-1]-s[1],tmp1.width());
    x_n.scores = tmp1.submatrix(s[1],0,x_n.scores.height(), x_n.scores.width());
    
    infra::matrix tmp4(x_n.distances);
    x_n.distances.resize(s[s.size()-1]-s[1],tmp4.width());
    x_n.distances = tmp4.submatrix(s[1],0,x_n.distances.height(), x_n.distances.width());
    
    //		//  The lines below need only when trying to force align full phone sequence
    //		for (uint i=0;i<keyword.size()-2;++i) 
    //			keyword[i] = keyword[i+1];
    //		keyword.resize(keyword.size()-2);
    
    uint tmp5 = s[1];
    for (uint i=0;i<s.size()-2;++i)
      s[i] = s[i+1]-tmp5;
    s.resize(s.size()-2);
    ///////////////////////////
  }
  
  end_frame = x_n.scores.height()-1;
  
  current_file++;
  
  return frame_offset;
}


std::ostream& operator<< (std::ostream& os, const IntVector& v)
{
  IntVector::const_iterator iter = v.begin();
  IntVector::const_iterator end = v.end();
  
  while(iter < end) {
    os << *iter << " ";
    ++iter;
  }
  return os;
}


// --------------------------  EOF ------------------------------------//
