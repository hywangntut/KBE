#ifndef KNOWLEDGEBASEEMBEDDING_FILE_RESOURCEFILE_H
#define KNOWLEDGEBASEEMBEDDING_FILE_RESOURCEFILE_H

#include <tr1/unordered_map>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace std;
using namespace tr1;

class ResourceFile
{
public:
	ResourceFile();
	ResourceFile(string resourceFilePath);
	bool IsExist();
	unordered_map<string, bool> GetElementList() const { return _elementList; };
	unordered_map<int, bool> GetHashList() const { return _hashList; };
	unordered_map<string, unordered_map<string, bool> > GetRelatedElementList() const { return _relatedElementList; };
	unordered_map<int, unordered_map<int, bool> > GetRelatedHashList() const { return _relatedHashList; };
	void GenerateRelatedElement(string resourceFilePath);
	void GenerateRelatedHash(unordered_map<string, int> hashList);

protected:
	static const int BUFFER_SIZE = 1000;

	bool _fileExist = false;
	unordered_map<string, bool> _elementList;
	unordered_map<int, bool> _hashList;
	unordered_map<string, unordered_map<string, bool> > _relatedElementList;
	unordered_map<int, unordered_map<int, bool> > _relatedHashList;

protected:
	void AddElementToList(string element);
	void AddHashToList(int hash);
	void AddRelatedElementPair(string keyElement, string valueElement);
	void AddRelatedHashPair(int keyHash, int valueHash);
};

#endif