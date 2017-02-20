#ifndef KNOWLEDGEBASEEMBEDDING_VOCABTABLE_H
#define KNOWLEDGEBASEEMBEDDING_VOCABTABLE_H

#include <tr1/unordered_map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Parameter.h"

using namespace std;
using namespace tr1;

#define VOCAB_HASH_TABLE_SIZE 30000000

struct Vocabulary
{
	char* word;
	long long count;
};

class VocabTable
{
public:
	int* _vocabHashTable;
	struct Vocabulary* _vocabTable;
	long long _vocabTableSize;
	long long _vocabCount;
	long long _totalWordCount;
	int _reduceWordCount;

public:
	VocabTable();
	int AddWord(char *word);
	int AddDictionary(unordered_map<string, bool> dictionary);
	int AddWordToVocab(char *word);
	unordered_map<string, int> GetWordHash();
	int SearchVocab(char *word);
	void SortVocab(int minWordCount);

private:
	int GetWordHash(char *word);
	void ReduceVocab();
	static int VocabCompare(const void *a, const void *b);
};

#endif