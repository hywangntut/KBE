#ifndef KNOWLEDGEBASEEMBEDDING_FILE_CORPUSFILE_H
#define KNOWLEDGEBASEEMBEDDING_FILE_CORPUSFILE_H

#include "FileReader.h"
#include "VocabTable.h"

using namespace std;

class CorpusFile
{
public:
	static void LoadVocab(Parameter* userSetting, VocabTable* vocabTable);
	static long long _corpusFileSize;
};

#endif