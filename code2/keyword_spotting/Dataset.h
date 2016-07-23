
#ifndef _MY_DATASET_H_
#define _MY_DATASET_H_

/************************************************************************
 Project:  Phoeneme Alignment
 Module:   Dataset Definitions
 Purpose:  Defines the data structs of instance and label
 Date:     25 Jan., 2005
 
 *************************** INCLUDE FILES ******************************/
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
#include <map>
#include <../learning_tools/infra2/infra.h>

#define MAX_LINE_SIZE 4096

class IntVector : public std::vector<int> {
public:
	unsigned int read(std::string &filename) {
		std::ifstream ifs(filename.c_str());
		// check input file stream
		if (!ifs.good()) {
			std::cerr << "Error: Unable to read IntVector from " << filename 
			<< std::endl;
			exit(-1);
		}
		// delete the vector
		clear();
		// read size from the stream
		int value;
		int num_values;
		if (ifs.good()) 
			ifs >> num_values;
		while (ifs.good() && num_values--) {
			ifs >> value;
			push_back(value);
		}
		ifs.close();
		return size();
	}
	
};

std::ostream& operator<< (std::ostream& os, const IntVector& v);

/***********************************************************************/

class StringVector : public std::vector<std::string> {
public:
	
	unsigned int read(std::string &filename) {
		std::ifstream ifs;
		char line[MAX_LINE_SIZE];
		ifs.open(filename.c_str());
		if (!ifs.is_open()) {
			std::cerr << "Error: Unable to open file list " << filename << std::endl;
			exit(-1);
		}    
		while (!ifs.eof()) {
			ifs.getline(line,MAX_LINE_SIZE);
			if (strcmp(line,""))
				push_back(std::string(line));
		}
		ifs.close();
		return size();
	}
};

/***********************************************************************/
class PhonemeSequence : public std::vector<int>
{
public:
	void read(std::string &filename);
	static void load_phoneme_map(std::string &phoneme_filename, 
															 std::string &silence_symbol);
	
public:
	static unsigned int num_phonemes;
	static std::map<std::string, int> phoneme2index;
	static std::map<int,std::string> index2phoneme;  
};

std::ostream& operator<< (std::ostream& os, const PhonemeSequence& y);

/***********************************************************************/

class StartTimeSequence : public std::vector<int>
{
public:
	unsigned int read(std::string &filename) {
		std::ifstream ifs(filename.c_str());
		// check input file stream
		if (!ifs.good()) {
			std::cerr << "Info: unable to read StartTimeSequence from " 
			<< filename << std::endl;
			return 0;
		}
		// delete the vector
		clear();
		// read size from the stream
		while (ifs.good()) {
			std::string value;
			ifs >> value;
			if (value == "") break;
			push_back(int(std::atoi(value.c_str())));
		}
		ifs.close();
		return size();
	}
};

std::ostream& operator<< (std::ostream& os, const StartTimeSequence& y);

/***********************************************************************/

class SpeechUtterance
	{
	public:
		void read(std::string &scores_filename, std::string &dists_filename);
		static void load_dist_stats(std::string &filename);
		unsigned long size();  
		
	public:
		infra::matrix scores;
		infra::matrix distances;
		static infra::vector dist_mean;
		static infra::vector dist_std;
	};

/***********************************************************************/

class Dataset 
	{
	public:
		Dataset(std::string& pos_filename, std::string& neg_filelist, 
						std::string& keyword_phoneme_filename, std::string& keyword_alignment_filename);
		Dataset(std::string& neg_filelist, std::string& keyword_phoneme_filename);
		void read(SpeechUtterance &x_p, SpeechUtterance &x_n, PhonemeSequence &keyword, 
							StartTimeSequence &s, int &end_frame, std::string scores_ext, bool remove_silence);
		int read(SpeechUtterance &x_n, PhonemeSequence &keyword, StartTimeSequence &s, 
						 int &end_frame, std::string scores_ext, bool remove_silence);
		unsigned long size() { return keyword_phoneme_list.size(); } 
		
	private:
		StringVector pos_file_list;
		StringVector neg_file_list;
		StringVector keyword_phoneme_list;
		StringVector keyword_alignment_list;
		int current_file;
	};

#endif // _MY_DATASET_H_
