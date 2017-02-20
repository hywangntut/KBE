#include "RCM.h"
#include "NeuralNetwork.h"

void* RCM::TrainModel(void* methodInfo)
{
	MethodInfo* info = (MethodInfo *)methodInfo;
	Parameter* userSetting = info->GetUserSetting();
	VocabTable* vocabTable = info->GetVocabTable();

	long long* completeWordCount = &NeuralNetwork::_rcmCompleteWordCount;
	float currentLambda = NeuralNetwork::_currentLambda;
	float lambda = userSetting->RCMLambda();
	int threadCount = userSetting->RCMThreadCount();
	unordered_map<string, unordered_map<string, bool> > relatedWord;
	unordered_map<int, unordered_map<int, bool> > relatedHash;
	if (userSetting->RCMResourceFilePath() != "")
	{
		relatedWord = NeuralNetwork::_rcmResourceFile->GetRelatedElementList();
		relatedHash = NeuralNetwork::_rcmResourceFile->GetRelatedHashList();
	}

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	long long d, word, last_word;
	long long l1, l2, c, target, label;
	float f, g;
	clock_t now;
	float *neu1 = (float *)calloc(userSetting->Dimension(), sizeof(float));
	float *gradientl1 = (float *)calloc(userSetting->Dimension(), sizeof(float));
	float *gradientl2 = (float *)calloc(userSetting->Dimension(), sizeof(float));
	int i = 0;

	unsigned long long s_next_random = (long long)info->GetThreadID();
	long long s_word_count = 0, s_last_word_count = 0;

	long long s_train_words_total = 0, s_begin = 0, s_end = 0;
	long long* s_wordIds = (long long*)calloc(1, sizeof(long long));
	string* s_wordList = new string[1];

	if (userSetting->RCMResourceFilePath() != "")
	{
		s_train_words_total = userSetting->RCMEpoch() * relatedWord.size();
		s_begin = relatedWord.size() / (long long)threadCount * info->GetThreadID();
		s_end = relatedWord.size() / (long long)threadCount * (info->GetThreadID() + 1);
		s_wordIds = (long long*)calloc(relatedWord.size(), sizeof(long long));
		s_wordList = new string[relatedWord.size()];
		for (unordered_map<string, unordered_map<string, bool> >::iterator iter_pair = relatedWord.begin(); iter_pair != relatedWord.end(); iter_pair++)
		{
			word = vocabTable->SearchVocab((char*)iter_pair->first.c_str());
			s_wordList[i] = iter_pair->first;
			s_wordIds[i++] = word;
		}
	}

	i = 0;
	double wordIC = 0.0f;
	double last_wordIC = 0.0f;

	if (userSetting->RCMResourceFilePath() != "")
	{
		// Do Synonym Training
		for (int s_ep = 0; s_ep < userSetting->RCMEpoch(); s_ep++)
		{
			for (i = s_begin; i < s_end; i++)
			{
				word = s_wordIds[i];
				if (word == -1)
				{
					continue;
				}
				unordered_map<string, unordered_map<string, bool> >::iterator iter_pair = relatedWord.find(s_wordList[i]);

				for (unordered_map<string, bool>::iterator iter = iter_pair->second.begin(); iter != iter_pair->second.end(); iter++)
				{
					last_word = vocabTable->SearchVocab((char*)iter->first.c_str());
					if (last_word == -1) continue;
					pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
					pthread_testcancel();
					pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

					if (userSetting->RCMMethod() == 3)
					{
						last_wordIC = NeuralNetwork::_icFile->GetInformationContent(vocabTable->_vocabTable[last_word].word);
					}
					l1 = last_word * userSetting->Dimension();
					for (c = 0; c < userSetting->Dimension(); c++) neu1[c] = 0;
					// NEGATIVE SAMPLING
					if (userSetting->NegativeSampleCount() > 0) for (d = 0; d < userSetting->NegativeSampleCount() + 1; d++)
					{
						if (d == 0)
						{
							target = word;
							label = 1;
						}
						else
						{
							for (int i = 0; i < vocabTable->_vocabCount; i++)
							{
								s_next_random = s_next_random * (unsigned long long)25214903917 + 11;
								target = NeuralNetwork::_unigramTable[(s_next_random >> 16) % UNIGRAM_TABLE_SIZE];
								if (relatedHash[word].find(target) == relatedHash[word].end())
								{
									break;
								}
							}
							label = 0;
						}
						// Skip Low-Feq pairs in Threshold method
						if (userSetting->RCMMethod() == 1)
						{
							if (vocabTable->_vocabTable[target].count < userSetting->RCMThreshold())
							{
								if (vocabTable->_vocabTable[last_word].count < userSetting->RCMThreshold())
								{
									continue;
								}
							}
						}
						if (userSetting->RCMMethod() == 3)
						{
							wordIC = NeuralNetwork::_icFile->GetInformationContent(vocabTable->_vocabTable[target].word);
						}
						l2 = target * userSetting->Dimension();
						f = 0;
						for (c = 0; c < userSetting->Dimension(); c++) f += NeuralNetwork::_predictMatrix[c + l1] * NeuralNetwork::_predictMatrix[c + l2];
						if (f > MAX_EXP) g = (label - 1) * currentLambda;
						else if (f < -MAX_EXP) g = (label - 0) * currentLambda;
						else g = (label - NeuralNetwork::_expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * currentLambda;

						if ((userSetting->RCMMethod() == 0) || (userSetting->RCMMethod() == 1))
						{
							for (c = 0; c < userSetting->Dimension(); c++) neu1[c] += g * NeuralNetwork::_predictMatrix[c + l2];
						}
						else if (userSetting->RCMMethod() == 2)
						{
							for (c = 0; c < userSetting->Dimension(); c++) neu1[c] += tanh((float)vocabTable->_vocabTable[target].count / vocabTable->_vocabTable[last_word].count) * g * NeuralNetwork::_predictMatrix[c + l2];
						}
						else if (userSetting->RCMMethod() == 3)
						{
							for (c = 0; c < userSetting->Dimension(); c++) neu1[c] += tanh((float)wordIC / last_wordIC) * g * NeuralNetwork::_predictMatrix[c + l2];
						}

						if (userSetting->RCMMethod() == 0)
						{
							for (c = 0; c < userSetting->Dimension(); c++) NeuralNetwork::_predictMatrix[c + l2] += g * NeuralNetwork::_predictMatrix[c + l1];
						}
						else if ((userSetting->RCMMethod() == 1) && (vocabTable->_vocabTable[target].count < userSetting->RCMThreshold()))
						{
							for (c = 0; c < userSetting->Dimension(); c++) NeuralNetwork::_predictMatrix[c + l2] += g * NeuralNetwork::_predictMatrix[c + l1];
						}
						else if (userSetting->RCMMethod() == 2)
						{
							for (c = 0; c < userSetting->Dimension(); c++) NeuralNetwork::_predictMatrix[c + l2] += tanh((float)vocabTable->_vocabTable[last_word].count / vocabTable->_vocabTable[target].count) * g * NeuralNetwork::_predictMatrix[c + l1];
						}
						else if (userSetting->RCMMethod() == 3)
						{
							for (c = 0; c < userSetting->Dimension(); c++) NeuralNetwork::_predictMatrix[c + l2] += tanh((float)last_wordIC / wordIC) * g * NeuralNetwork::_predictMatrix[c + l1];
						}
					}
					if ((userSetting->RCMMethod() == 0) || (userSetting->RCMMethod() == 2) || (userSetting->RCMMethod() == 3))
					{
						for (c = 0; c < userSetting->Dimension(); c++) NeuralNetwork::_predictMatrix[c + l1] += neu1[c];
					}
					else if ((userSetting->RCMMethod() == 1) && (vocabTable->_vocabTable[last_word].count < userSetting->RCMThreshold()))
					{
						for (c = 0; c < userSetting->Dimension(); c++) NeuralNetwork::_predictMatrix[c + l1] += neu1[c];
					}
				}
				s_word_count++;
				if ((s_word_count - s_last_word_count) % 5000 == 0)
				{
					*completeWordCount += s_word_count - s_last_word_count;
					now = clock();
					printf("%cLambda: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, currentLambda,
						*completeWordCount / (float)(s_train_words_total + 1) * 100,
						*completeWordCount / ((float)(now - NeuralNetwork::_startTime + 1) / (float)CLOCKS_PER_SEC * 1000));
					fflush(stdout);
					currentLambda = lambda * (1 - *completeWordCount / (float)(s_train_words_total + 1));
					if (currentLambda < lambda * 0.0001) currentLambda = lambda * 0.0001;
					s_word_count = s_last_word_count;
				}
			}
		}
	}

	if (userSetting->RCMResourceFilePath() != "")
	{
		delete[] s_wordList;
		free(s_wordIds);
	}

	free(gradientl2);
	free(gradientl1);
	free(neu1);
	pthread_exit(NULL);
}