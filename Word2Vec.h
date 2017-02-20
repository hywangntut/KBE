#ifndef KNOWLEDGEBASEEMBEDDING_METHOD_WORD2VEC_H
#define KNOWLEDGEBASEEMBEDDING_METHOD_WORD2VEC_H

#include <math.h>
#include "Parameter.h"
#include "CorpusFile.h"
#include "FileReader.h"

using namespace std;

class NeuralNetwork;

class Word2Vec
{
public:
	static void* TrainModel(void* methodInfo);
};

#endif