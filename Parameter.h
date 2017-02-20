#ifndef KNOWLEDGEBASEEMBEDDING_PARAMETER_H
#define KNOWLEDGEBASEEMBEDDING_PARAMETER_H

#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define MAX_WORD_LENGTH 1000
#define MAX_SENTENCE_LENGTH 1000

class Parameter
{
public:
	Parameter();
	Parameter(int argc, char** argv);

	string CorpusFilePath() const { return _corpusFilePath; };
	int Dimension() const { return _dimension; };
	int MinWordCount() const { return _minWordCount; };
	int NegativeSampleCount() const { return _negativeSampleCount; };

	int UseWord2Vec() const { return _useWord2Vec; };
	float Word2VecAlpha() const { return _word2VecAlpha; };
	int Word2VecEpoch() const { return _word2VecEpoch; };
	float Word2VecSubsampleThreshold() const { return _word2VecSubsampleThreshold; };
	int Word2VecThreadCount() const { return _word2VecThreadCount; };
	int Word2VecWindowSize() const { return _word2VecWindowSize; };

	int UseRCM() const { return _useRCM; };
	int RCMEpoch() const { return _rcmEpoch; };
	float RCMLambda() const { return _rcmLambda; };
	string RCMResourceFilePath() const { return _rcmResourceFilePath; };
	string RCMICFilePath() const { return _rcmICFilePath; };
	int RCMThreadCount() const { return _rcmThreadCount; };
	int RCMMethod() const { return _rcmMethod; };
	int RCMThreshold() const { return _rcmThreshold; };

	int UsePretrainModel() const { return _usePretrainModel; };
	string PretrainModelFilePath() const { return _pretrainModelFilePath; };

	string OutputModelFilePath() const { return _outputModelFilePath; };

private:
	string _corpusFilePath;
	int _dimension;
	int _minWordCount;
	int _negativeSampleCount;

	int _useWord2Vec;
	float _word2VecAlpha;
	int _word2VecEpoch;
	float _word2VecSubsampleThreshold;
	int _word2VecThreadCount;
	int _word2VecWindowSize;

	int _useRCM;
	int _rcmEpoch;
	float _rcmLambda;
	string _rcmResourceFilePath;
	string _rcmICFilePath;
	int _rcmThreadCount;
	int _rcmMethod;
	int _rcmThreshold;

	int _usePretrainModel;
	string _pretrainModelFilePath;

	string _outputModelFilePath;

private:
	int DefaultParameter();
	int IdentifyArgument(char* arg, int argc, char** argv);
};

#endif