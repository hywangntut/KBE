#ifndef KNOWLEDGEBASEEMBEDDING_FILE_FILEREADER_H
#define KNOWLEDGEBASEEMBEDDING_FILE_FILEREADER_H

#include <fstream>
#include <sstream>
#include "Parameter.h"
#include "VocabTable.h"

using namespace std;

class FileReader
{
public:
	static void ReadElement(FILE *file, char *element);
	static int ReadElementHash(FILE *file, VocabTable* vocabTable);
};

#endif