#include "Parameter.h"

Parameter::Parameter()
{
}

Parameter::Parameter(int argc, char **argv)
{
	DefaultParameter();

	int i = 0;
	if ((i = IdentifyArgument((char *)"-corpus", argc, argv)) > 0) _corpusFilePath = string(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-dimension", argc, argv)) > 0) _dimension = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-min-count", argc, argv)) > 0) _minWordCount = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-negative", argc, argv)) > 0) _negativeSampleCount = atoi(argv[i + 1]);

	if ((i = IdentifyArgument((char *)"-word2vec", argc, argv)) > 0) _useWord2Vec = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-word2vec-alpha", argc, argv)) > 0) _word2VecAlpha = atof(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-word2vec-epoch", argc, argv)) > 0) _word2VecEpoch = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-word2vec-subsample", argc, argv)) > 0) _word2VecSubsampleThreshold = atof(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-word2vec-thread", argc, argv)) > 0) _word2VecThreadCount = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-word2vec-window", argc, argv)) > 0) _word2VecWindowSize = atoi(argv[i + 1]);

	if ((i = IdentifyArgument((char *)"-rcm", argc, argv)) > 0) _useRCM = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-rcm-epoch", argc, argv)) > 0) _rcmEpoch = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-rcm-lambda", argc, argv)) > 0) _rcmLambda = atof(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-rcm-resource", argc, argv)) > 0) _rcmResourceFilePath = string(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-rcm-ic", argc, argv)) > 0) _rcmICFilePath = string(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-rcm-thread", argc, argv)) > 0) _rcmThreadCount = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-rcm-method", argc, argv)) > 0) _rcmMethod = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-rcm-threshold", argc, argv)) > 0) _rcmThreshold = atoi(argv[i + 1]);

	if ((i = IdentifyArgument((char *)"-pretrain", argc, argv)) > 0) _usePretrainModel = atoi(argv[i + 1]);
	if ((i = IdentifyArgument((char *)"-pretrain-model", argc, argv)) > 0) _pretrainModelFilePath = string(argv[i + 1]);

	if ((i = IdentifyArgument((char *)"-output-model", argc, argv)) > 0) _outputModelFilePath = string(argv[i + 1]);
}

int Parameter::DefaultParameter()
{
	_corpusFilePath = "";
	_dimension = 300;
	_minWordCount = 0;
	_negativeSampleCount = 15;

	_useWord2Vec = 0;
	_word2VecAlpha = 0.025;
	_word2VecEpoch = 1;
	_word2VecSubsampleThreshold = 0;
	_word2VecThreadCount = 0;
	_word2VecWindowSize = 5;

	_useRCM = 0;
	_rcmEpoch = 0;
	_rcmLambda = 0.01;
	_rcmResourceFilePath = "";
	_rcmICFilePath = "";
	_rcmThreadCount = 0;
	_rcmMethod = 0;
	_rcmThreshold = 0;

	_usePretrainModel = 0;
	_pretrainModelFilePath = "";

	_outputModelFilePath = "";

	return 0;
}

int Parameter::IdentifyArgument(char* arg, int argc, char** argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(arg, argv[i]))
		{
			if (i == argc - 1)
			{
				cout << "Argument missing for " << arg << endl;
				exit(0);
			}
			return i;
		}
	}
	return -1;
}