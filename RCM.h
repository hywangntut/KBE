#ifndef KNOWLEDGEBASEEMBEDDING_METHOD_JRCM_H
#define KNOWLEDGEBASEEMBEDDING_METHOD_JRCM_H

#include <vector>
#include <math.h>
#include "Parameter.h"
#include "ResourceFile.h"
#include "CorpusFile.h"

using namespace std;
using namespace tr1;

class NeuralNetwork;

class RCM
{
public:
	static void* TrainModel(void* methodInfo);
};

#endif