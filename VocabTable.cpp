#include "VocabTable.h"

VocabTable::VocabTable()
{
	_vocabTableSize = 1000;
	_vocabCount = 0;
	_totalWordCount = 0;
	_reduceWordCount = 1;
	_vocabTable = (struct Vocabulary *)calloc(_vocabTableSize, sizeof(struct Vocabulary));
	_vocabHashTable = (int *)calloc(VOCAB_HASH_TABLE_SIZE, sizeof(int));
	for (long long i = 0; i < VOCAB_HASH_TABLE_SIZE; i++)
	{
		_vocabHashTable[i] = -1;
	}
	AddWordToVocab((char *)"</s>");
}

int VocabTable::AddWord(char *word)
{
	_totalWordCount++;
	int wordIndex = SearchVocab(word);
	if (wordIndex == -1)
	{
		wordIndex = AddWordToVocab(word);
		_vocabTable[wordIndex].count = 1;
	}
	else
	{
		_vocabTable[wordIndex].count++;
	}
	if (_vocabCount > VOCAB_HASH_TABLE_SIZE * 0.7)
	{
		ReduceVocab();
	}
	return 0;
}

int VocabTable::AddDictionary(unordered_map<string, bool> dictionary)
{
	for (unordered_map<string, bool>::iterator dictionaryIter = dictionary.begin(); dictionaryIter != dictionary.end(); dictionaryIter++)
	{
		int wordIndex = SearchVocab((char *)dictionaryIter->first.c_str());
		if (wordIndex == -1)
		{
			_totalWordCount++;
			wordIndex = AddWordToVocab((char *)dictionaryIter->first.c_str());
			_vocabTable[wordIndex].count = 1;
		}
		if (_vocabCount > VOCAB_HASH_TABLE_SIZE * 0.7)
		{
			ReduceVocab();
		}
	}
	return 0;
}

int VocabTable::AddWordToVocab(char *word)
{
	unsigned int hash = 0;
	unsigned int length = strlen(word) + 1;
	if (length > MAX_WORD_LENGTH)
	{
		length = MAX_WORD_LENGTH;
	}
	_vocabTable[_vocabCount].word = (char *)calloc(length, sizeof(char));
	strcpy(_vocabTable[_vocabCount].word, word);
	_vocabTable[_vocabCount].count = 0;
	_vocabCount++;

	if (_vocabCount + 2 >= _vocabTableSize)
	{
		_vocabTableSize += 1000;
		_vocabTable = (struct Vocabulary *)realloc(_vocabTable, _vocabTableSize * sizeof(struct Vocabulary));
	}
	hash = GetWordHash(word);
	while (_vocabHashTable[hash] != -1)
	{
		hash = (hash + 1) % VOCAB_HASH_TABLE_SIZE;
	}
	_vocabHashTable[hash] = _vocabCount - 1;
	return _vocabCount - 1;
}

int VocabTable::GetWordHash(char *word)
{
	unsigned long long i = 0, hash = 0;
	for (i = 0; i < strlen(word); i++)
	{
		hash = hash * 257 + word[i];
	}
	hash = hash % VOCAB_HASH_TABLE_SIZE;
	return hash;
}

unordered_map<string, int> VocabTable::GetWordHash()
{
	unordered_map<string, int> wordHash;
	for (int i = 0; i < _vocabCount; i++)
	{
		wordHash[_vocabTable[i].word] = i;
	}
	return wordHash;
}

int VocabTable::SearchVocab(char *word)
{
	unsigned int hash = GetWordHash(word);
	while (true)
	{
		if (_vocabHashTable[hash] == -1)
		{
			return -1;
		}
		if (!strcmp(word, _vocabTable[_vocabHashTable[hash]].word))
		{
			return _vocabHashTable[hash];
		}
		hash = (hash + 1) % VOCAB_HASH_TABLE_SIZE;
	}
	return -1;
}

void VocabTable::ReduceVocab()
{
	int i = 0, j = 0;
	unsigned int hash = 0;
	for (i = 0; i < _vocabCount; i++)
	{
		if (_vocabTable[i].count > _reduceWordCount)
		{
			_vocabTable[j].count = _vocabTable[i].count;
			_vocabTable[j].word = _vocabTable[i].word;
			j++;
		}
		else
		{
			free(_vocabTable[i].word);
		}
	}
	_vocabCount = j;

	for (i = 0; i < VOCAB_HASH_TABLE_SIZE; i++)
	{
		_vocabHashTable[i] = -1;
	}
	for (i = 0; i < _vocabCount; i++)
	{
		hash = GetWordHash(_vocabTable[i].word);
		while (_vocabHashTable[hash] != -1)
		{
			hash = (hash + 1) % VOCAB_HASH_TABLE_SIZE;
		}
		_vocabHashTable[hash] = i;
	}
	fflush(stdout);
	_reduceWordCount++;
}

void VocabTable::SortVocab(int minWordCount)
{
	int i = 0;
	unsigned int hash = 0;
	qsort(&_vocabTable[1], _vocabCount - 1, sizeof(struct Vocabulary), VocabCompare);
	for (i = 0; i < VOCAB_HASH_TABLE_SIZE; i++)
	{
		_vocabHashTable[i] = -1;
	}

	int size = _vocabCount;
	_totalWordCount = 0;
	for (i = 0; i < size; i++)
	{
		if (_vocabTable[i].count < minWordCount)
		{
			_vocabCount--;
			free(_vocabTable[_vocabCount].word);
		}
		else
		{
			hash = GetWordHash(_vocabTable[i].word);
			while (_vocabHashTable[hash] != -1)
			{
				hash = (hash + 1) % VOCAB_HASH_TABLE_SIZE;
			}
			_vocabHashTable[hash] = i;
			_totalWordCount += _vocabTable[i].count;
		}
	}
	_vocabTable = (struct Vocabulary *)realloc(_vocabTable, (_vocabCount + 1) * sizeof(struct Vocabulary));
}

int VocabTable::VocabCompare(const void *a, const void *b)
{
	return ((struct Vocabulary *)b)->count - ((struct Vocabulary *)a)->count;
}