#ifndef KNOWLEDGEBASEEMBEDDING_NEURALNETWORK_H
#define KNOWLEDGEBASEEMBEDDING_NEURALNETWORK_H

#include "Parameter.h"
#include "CorpusFile.h"
#include "ICFile.h"
#include "ResourceFile.h"
#include "MethodInfo.h"
#include "Word2Vec.h"
#include "RCM.h"

#define UNIGRAM_TABLE_SIZE 100000000
#define EXP_TABLE_SIZE 1000
#define MAX_EXP 6

using namespace std;
using namespace tr1;

class NeuralNetwork
{
public:
	static Parameter* _userSetting;
	static VocabTable* _vocabTable;
	static float* _embeddingMatrix;
	static float* _predictMatrix;
	static float* _expTable;
	static int* _unigramTable;
	static clock_t _startTime;

	static long long _word2VecCompleteWordCount;
	static float _currentAlpha;

	static long long _rcmCompleteWordCount;
	static float _currentLambda;

	static ResourceFile* _rcmResourceFile;
	static ICFile* _icFile;

	static void TrainModel(Parameter* userSetting);

private:
	static void InitUnigramTable();
	static void InitNet();
	static void TrainModelByMethod();
	static int LoadPretrainEmbedding();
};

#endif