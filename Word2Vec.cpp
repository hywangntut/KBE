#include "Word2Vec.h"
#include "NeuralNetwork.h"

void* Word2Vec::TrainModel(void* methodInfo)
{
	MethodInfo* info = (MethodInfo *)methodInfo;
	Parameter* userSetting = info->GetUserSetting();
	VocabTable* vocabTable = info->GetVocabTable();

	if (NeuralNetwork::_userSetting->UseWord2Vec())
	{
		long long a, b, d, word, last_word, sentence_length = 0, sentence_position = 0;
		long long word_count = 0, last_word_count = 0, sen[MAX_SENTENCE_LENGTH + 1];
		long long l2, c, target, label;
		unsigned long long next_random = (long long)info->GetThreadID();
		float f, g;
		clock_t now;
		float *neu1 = (float *)calloc(NeuralNetwork::_userSetting->Dimension(), sizeof(float));
		float *neu1e = (float *)calloc(NeuralNetwork::_userSetting->Dimension(), sizeof(float));
		FILE *fi = fopen(NeuralNetwork::_userSetting->CorpusFilePath().c_str(), "rb");

		for (int ep = 0; ep < userSetting->Word2VecEpoch(); ep++)
		{
			fseek(fi, CorpusFile::_corpusFileSize / (long long)NeuralNetwork::_userSetting->Word2VecThreadCount() * (long long)info->GetThreadID(), SEEK_SET);

			while (true)
			{
				if (word_count - last_word_count > 10000)
				{
					NeuralNetwork::_word2VecCompleteWordCount += word_count - last_word_count;
					last_word_count = word_count;
					now = clock();
					printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, NeuralNetwork::_currentAlpha,
						NeuralNetwork::_word2VecCompleteWordCount / (float)(vocabTable->_totalWordCount + 1) * 100,
						NeuralNetwork::_word2VecCompleteWordCount / ((float)(now - NeuralNetwork::_startTime + 1) / (float)CLOCKS_PER_SEC * 1000));
					fflush(stdout);
					NeuralNetwork::_currentAlpha = NeuralNetwork::_userSetting->Word2VecAlpha() * (1 - NeuralNetwork::_word2VecCompleteWordCount / (float)(vocabTable->_totalWordCount + 1));
					if (NeuralNetwork::_currentAlpha < NeuralNetwork::_userSetting->Word2VecAlpha() * 0.0001) NeuralNetwork::_currentAlpha = NeuralNetwork::_userSetting->Word2VecAlpha() * 0.0001;
				}
				if (sentence_length == 0)
				{
					while (true)
					{
						word = FileReader::ReadElementHash(fi, vocabTable);
						if (feof(fi)) break;
						if (word == -1) continue;
						word_count++;
						if (word == 0) break;
						// The subsampling randomly discards frequent words while keeping the ranking same
						if (NeuralNetwork::_userSetting->Word2VecSubsampleThreshold() > 0)
						{
							float ran = (sqrt(vocabTable->_vocabTable[word].count / (NeuralNetwork::_userSetting->Word2VecSubsampleThreshold() * vocabTable->_totalWordCount)) + 1) * (NeuralNetwork::_userSetting->Word2VecSubsampleThreshold() * vocabTable->_totalWordCount) / vocabTable->_vocabTable[word].count;
							next_random = next_random * (unsigned long long)25214903917 + 11;
							if (ran < (next_random & 0xFFFF) / (float)65536) continue;
						}
						sen[sentence_length] = word;
						sentence_length++;
						if (sentence_length >= MAX_SENTENCE_LENGTH) break;
					}
					sentence_position = 0;
				}
				if (feof(fi)) break;
				if (sentence_length == 0) continue;
				if (word_count > vocabTable->_totalWordCount / NeuralNetwork::_userSetting->Word2VecThreadCount()) break;
				word = sen[sentence_position];
				if (word == -1) continue;
				for (c = 0; c < NeuralNetwork::_userSetting->Dimension(); c++) neu1[c] = 0;
				for (c = 0; c < NeuralNetwork::_userSetting->Dimension(); c++) neu1e[c] = 0;
				next_random = next_random * (unsigned long long)25214903917 + 11;
				b = next_random % NeuralNetwork::_userSetting->Word2VecWindowSize();
				if (NeuralNetwork::_userSetting->UseWord2Vec())
				{
					// in -> hidden
					for (a = b; a < NeuralNetwork::_userSetting->Word2VecWindowSize() * 2 + 1 - b; a++) if (a != NeuralNetwork::_userSetting->Word2VecWindowSize())
					{
						c = sentence_position - NeuralNetwork::_userSetting->Word2VecWindowSize() + a;
						if (c < 0) continue;
						if (c >= sentence_length) continue;
						last_word = sen[c];
						if (last_word == -1) continue;
						for (c = 0; c < NeuralNetwork::_userSetting->Dimension(); c++) neu1[c] += NeuralNetwork::_embeddingMatrix[c + last_word * NeuralNetwork::_userSetting->Dimension()];
					}
					// NEGATIVE SAMPLING
					if (NeuralNetwork::_userSetting->NegativeSampleCount() > 0) for (d = 0; d < NeuralNetwork::_userSetting->NegativeSampleCount() + 1; d++)
					{
						if (d == 0)
						{
							target = word;
							label = 1;
						}
						else
						{
							next_random = next_random * (unsigned long long)25214903917 + 11;
							target = NeuralNetwork::_unigramTable[(next_random >> 16) % UNIGRAM_TABLE_SIZE];
							if (target == 0) target = next_random % (vocabTable->_vocabCount - 1) + 1;
							if (target == word) continue;
							label = 0;
						}
						l2 = target * NeuralNetwork::_userSetting->Dimension();
						f = 0;
						for (c = 0; c < NeuralNetwork::_userSetting->Dimension(); c++) f += neu1[c] * NeuralNetwork::_predictMatrix[c + l2];
						if (f > MAX_EXP) g = (label - 1) * NeuralNetwork::_currentAlpha;
						else if (f < -MAX_EXP) g = (label - 0) * NeuralNetwork::_currentAlpha;
						else g = (label - NeuralNetwork::_expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * NeuralNetwork::_currentAlpha;
						for (c = 0; c < NeuralNetwork::_userSetting->Dimension(); c++) neu1e[c] += g * NeuralNetwork::_predictMatrix[c + l2];
						for (c = 0; c < NeuralNetwork::_userSetting->Dimension(); c++) NeuralNetwork::_predictMatrix[c + l2] += g * neu1[c];
					}
					// hidden -> in
					for (a = b; a < NeuralNetwork::_userSetting->Word2VecWindowSize() * 2 + 1 - b; a++) if (a != NeuralNetwork::_userSetting->Word2VecWindowSize()) {
						c = sentence_position - NeuralNetwork::_userSetting->Word2VecWindowSize() + a;
						if (c < 0) continue;
						if (c >= sentence_length) continue;
						last_word = sen[c];
						if (last_word == -1) continue;
						for (c = 0; c < NeuralNetwork::_userSetting->Dimension(); c++) NeuralNetwork::_embeddingMatrix[c + last_word * NeuralNetwork::_userSetting->Dimension()] += neu1e[c];
					}
				}
				sentence_position++;
				if (sentence_position >= sentence_length)
				{
					sentence_length = 0;
					continue;
				}
			}
		}

		fclose(fi);
		free(neu1);
		free(neu1e);
	}
	pthread_exit(NULL);
}