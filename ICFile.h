#ifndef KNOWLEDGEBASEEMBEDDING_FILE_ICFILE_H
#define KNOWLEDGEBASEEMBEDDING_FILE_ICFILE_H

#include <tr1/unordered_map>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace std;
using namespace tr1;

class ICFile
{
public:
	ICFile();
	ICFile(string icFilePath);
	bool IsExist();
	double GetInformationContent(string word);

protected:
	static const int BUFFER_SIZE = 1000;

	bool _fileExist = false;
	unordered_map<string, double> _informationContent;
};

#endif