#include "NeuralNetwork.h"

Parameter* NeuralNetwork::_userSetting;
VocabTable* NeuralNetwork::_vocabTable;
float* NeuralNetwork::_embeddingMatrix;
float* NeuralNetwork::_predictMatrix;
float* NeuralNetwork::_expTable;
int* NeuralNetwork::_unigramTable;
clock_t NeuralNetwork::_startTime;

long long NeuralNetwork::_word2VecCompleteWordCount;
float NeuralNetwork::_currentAlpha;

long long NeuralNetwork::_rcmCompleteWordCount;
float NeuralNetwork::_currentLambda;

ResourceFile* NeuralNetwork::_rcmResourceFile;
ICFile* NeuralNetwork::_icFile;

void NeuralNetwork::TrainModel(Parameter* userSetting)
{
	cout << "Start training model: " + userSetting->OutputModelFilePath() << endl;

	if (userSetting->OutputModelFilePath() == "")
	{
		cout << "ERROR: Output model file path incorrect!" << endl;
		return;
	}

	_userSetting = userSetting;
	_vocabTable = new VocabTable();

	_word2VecCompleteWordCount = 0;
	_currentAlpha = userSetting->Word2VecAlpha();

	_rcmCompleteWordCount = 0;
	_currentLambda = userSetting->RCMLambda();

	_expTable = (float *)malloc((EXP_TABLE_SIZE + 1) * sizeof(float));
	for (int i = 0; i < EXP_TABLE_SIZE; i++)
	{
		_expTable[i] = exp((i / (float)EXP_TABLE_SIZE * 2 - 1) * MAX_EXP);
		_expTable[i] = _expTable[i] / (_expTable[i] + 1);
	}

	CorpusFile::LoadVocab(userSetting, _vocabTable);

	if (userSetting->RCMResourceFilePath() != "")
	{
		_rcmResourceFile = new ResourceFile(userSetting->RCMResourceFilePath());
		if (!_rcmResourceFile->IsExist())
		{
			cout << "ERROR: Synonym file not found!" << endl;
			exit(1);
		}
	}

	_vocabTable->SortVocab(userSetting->MinWordCount());

	if (userSetting->RCMResourceFilePath() != "")
	{
		_rcmResourceFile->GenerateRelatedHash(_vocabTable->GetWordHash());
	}

	if (userSetting->RCMICFilePath() != "")
	{
		_icFile = new ICFile(userSetting->RCMICFilePath());
		if (!_icFile->IsExist())
		{
			cout << "ERROR: IC file not found!" << endl;
			exit(1);
		}
	}

	InitNet();
	if (userSetting->NegativeSampleCount() > 0)
	{
		InitUnigramTable();
	}

	TrainModelByMethod();

	if (!userSetting->UseRCM())
	{
		FILE *outputBinaryFile;
		outputBinaryFile = fopen((userSetting->OutputModelFilePath() + ".bin").c_str(), "wb");
		fprintf(outputBinaryFile, "%lld %d\n", _vocabTable->_vocabCount, userSetting->Dimension());
		for (long long i = 0; i < _vocabTable->_vocabCount; i++)
		{
			fprintf(outputBinaryFile, "%s ", _vocabTable->_vocabTable[i].word);
			for (int j = 0; j < userSetting->Dimension(); j++)
			{
				fwrite(&_embeddingMatrix[i * userSetting->Dimension() + j], sizeof(float), 1, outputBinaryFile);
			}
			for (int j = 0; j < userSetting->Dimension(); j++)
			{
				fwrite(&_predictMatrix[i * userSetting->Dimension() + j], sizeof(float), 1, outputBinaryFile);
			}
			fprintf(outputBinaryFile, "\n");
		}
		fclose(outputBinaryFile);
	}

	FILE *outputWordsFile;
	outputWordsFile = fopen((userSetting->OutputModelFilePath() + ".words").c_str(), "wb");
	fprintf(outputWordsFile, "%lld %d\n", _vocabTable->_vocabCount, userSetting->Dimension());
	for (long long i = 0; i < _vocabTable->_vocabCount; i++)
	{
		fprintf(outputWordsFile, "%s ", _vocabTable->_vocabTable[i].word);
		for (int j = 0; j < userSetting->Dimension(); j++)
		{
			fprintf(outputWordsFile, "%lf ", _predictMatrix[i * userSetting->Dimension() + j]);
		}
		fprintf(outputWordsFile, "\n");
	}
	fclose(outputWordsFile);

	cout << "Complete training model: " + userSetting->OutputModelFilePath() << endl;

	if (userSetting->RCMResourceFilePath() != "")
	{
		delete _rcmResourceFile;
		_rcmResourceFile = NULL;
	}
}

void NeuralNetwork::TrainModelByMethod()
{
	_startTime = clock();
	int totalThreadCount = _userSetting->Word2VecThreadCount() + _userSetting->RCMThreadCount();
	pthread_t* trainThread = (pthread_t *)malloc(totalThreadCount * sizeof(pthread_t));
	MethodInfo** methodInfo = (MethodInfo **)malloc(totalThreadCount * sizeof(MethodInfo *));

	int threadId = 0;
	for (int i = 0; i < _userSetting->Word2VecThreadCount(); i++)
	{
		methodInfo[threadId] = new MethodInfo(_userSetting, i, _vocabTable);
		pthread_create(&trainThread[threadId], NULL, Word2Vec::TrainModel, methodInfo[threadId]);
		threadId++;
	}
	if (_userSetting->RCMResourceFilePath() != "")
	{
		methodInfo[threadId] = new MethodInfo(_userSetting, 0, _vocabTable);
		if (_userSetting->RCMResourceFilePath() != "")
		{
			methodInfo[threadId]->SetRCMResource(_rcmResourceFile);
		}
		pthread_create(&trainThread[threadId], NULL, RCM::TrainModel, (void *)methodInfo[threadId]);
	}

	for (int i = 0; i < totalThreadCount; i++)
	{
		if ((_userSetting->UseWord2Vec() != 0) && (i >= _userSetting->Word2VecThreadCount()))
		{
			pthread_cancel(trainThread[i]);
		}
		else
		{
			pthread_join(trainThread[i], NULL);
		}
		delete methodInfo[i];
	}
	delete methodInfo;
	delete trainThread;
}

void NeuralNetwork::InitUnigramTable()
{
	int i = 0, j = 0;
	long long train_words_pow = 0;
	float d1, power = 0.75;
	_unigramTable = (int *)malloc(UNIGRAM_TABLE_SIZE * sizeof(int));
	for (i = 0; i < _vocabTable->_vocabCount; i++) train_words_pow += pow(_vocabTable->_vocabTable[i].count, power);
	d1 = pow(_vocabTable->_vocabTable[j].count, power) / (float)train_words_pow;
	for (i = 0; i < UNIGRAM_TABLE_SIZE; i++)
	{
		_unigramTable[i] = j;
		if (i / (float)UNIGRAM_TABLE_SIZE > d1)
		{
			j++;
			d1 += pow(_vocabTable->_vocabTable[j].count, power) / (float)train_words_pow;
		}
		if (j >= _vocabTable->_vocabCount)
		{
			j = (int)_vocabTable->_vocabCount - 1;
		}
	}
}

void NeuralNetwork::InitNet()
{
	long long a, b;
	a = posix_memalign((void **)&_embeddingMatrix, 128, (long long)_vocabTable->_vocabCount * _userSetting->Dimension() * sizeof(float));
	if (_embeddingMatrix == NULL) { printf("Memory allocation failed\n"); exit(1); }
	a = posix_memalign((void **)&_predictMatrix, 128, (long long)_vocabTable->_vocabCount * _userSetting->Dimension() * sizeof(float));
	if (_predictMatrix == NULL) { printf("Memory allocation failed\n"); exit(1); }

	for (b = 0; b < _userSetting->Dimension(); b++) for (a = 0; a < _vocabTable->_vocabCount; a++)
		_embeddingMatrix[a * _userSetting->Dimension() + b] = (rand() / (float)RAND_MAX - 0.5) / _userSetting->Dimension();
	for (b = 0; b < _userSetting->Dimension(); b++) for (a = 0; a < _vocabTable->_vocabCount; a++)
		_predictMatrix[a * _userSetting->Dimension() + b] = (rand() / (float)RAND_MAX - 0.5) / _userSetting->Dimension();

	if (_userSetting->UsePretrainModel()) LoadPretrainEmbedding();
}

int NeuralNetwork::LoadPretrainEmbedding()
{
	long long wordCount, dimension;

	FILE *pretrainModelFile = fopen(_userSetting->PretrainModelFilePath().c_str(), "rb");
	if (pretrainModelFile == NULL)
	{
		cout << "ERROR: Pretrain Model file not found!" << endl;
		return -1;
	}

	fscanf(pretrainModelFile, "%lld", &wordCount);
	fscanf(pretrainModelFile, "%lld", &dimension);
	if (dimension != _userSetting->Dimension())
	{
		cout << "ERROR: Inconsistent dimensions!" << endl;
		exit(-1);
	}

	char buffer[MAX_WORD_LENGTH];
	char dummyBuffer;
	float dummyEmbedding;
	for (int i = 0; i < wordCount; i++)
	{
		fscanf(pretrainModelFile, "%s%c", buffer, &dummyBuffer);
		int wordHash = _vocabTable->SearchVocab(buffer);
		if (wordHash == -1)
		{
			for (int j = 0; j < dimension * 2; j++) fread((void*)(&dummyEmbedding), sizeof(float), 1, pretrainModelFile);
		}
		else
		{
			for (int j = 0; j < dimension; j++) fread((void*)(&_embeddingMatrix[wordHash * dimension + j]), sizeof(_embeddingMatrix[wordHash * dimension + j]), 1, pretrainModelFile);
			for (int j = 0; j < dimension; j++) fread((void*)(&_predictMatrix[wordHash * dimension + j]), sizeof(_predictMatrix[wordHash * dimension + j]), 1, pretrainModelFile);
		}
	}
	fclose(pretrainModelFile);
	return 0;
}