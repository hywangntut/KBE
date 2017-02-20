#include "CorpusFile.h"

long long CorpusFile::_corpusFileSize = 0;

void CorpusFile::LoadVocab(Parameter* userSetting, VocabTable* vocabTable)
{
	char word[MAX_WORD_LENGTH];
	FILE *corpusFile;
	corpusFile = fopen(userSetting->CorpusFilePath().c_str(), "rb");
	if (corpusFile == NULL)
	{
		cout << "ERROR: Corpus file not found!" << endl;
		exit(1);
	}

	while (true)
	{
		FileReader::ReadElement(corpusFile, word);
		if (feof(corpusFile))
		{
			break;
		}
		if (vocabTable->_totalWordCount % 100000 == 0)
		{
			printf("%lldK%c", vocabTable->_totalWordCount / 1000, 13);
			fflush(stdout);
		}
		vocabTable->AddWord(word);
	}

	printf("Vocab size: %lld\n", vocabTable->_vocabCount);
	printf("Words in corpus: %lld\n", vocabTable->_totalWordCount);
	_corpusFileSize = ftell(corpusFile);
	fclose(corpusFile);
}