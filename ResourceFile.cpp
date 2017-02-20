#include "ResourceFile.h"

ResourceFile::ResourceFile()
{
}

ResourceFile::ResourceFile(string resourceFilePath)
{
	GenerateRelatedElement(resourceFilePath);
}

bool ResourceFile::IsExist()
{
	return _fileExist;
}

void ResourceFile::GenerateRelatedElement(string resourceFilePath)
{
	ifstream resourceFile(resourceFilePath.c_str());
	char buffer[BUFFER_SIZE] = " ";
	string keyElement = "", valueElement = "";

	_fileExist = resourceFile.good();
	if (_fileExist)
	{
		while (strcmp(buffer, ""))
		{
			resourceFile.getline(buffer, BUFFER_SIZE, '\n');
			istringstream relatedElementPair(buffer);
			relatedElementPair >> keyElement;
			if (keyElement != "")
			{
				AddElementToList(keyElement);
				while (!relatedElementPair.eof())
				{
					relatedElementPair >> valueElement;
					if (valueElement != "")
					{
						AddElementToList(valueElement);
						AddRelatedElementPair(keyElement, valueElement);
						AddRelatedElementPair(valueElement, keyElement);
					}
				}
			}
		}
	}
	resourceFile.close();
}

void ResourceFile::GenerateRelatedHash(unordered_map<string, int> hashList)
{
	for (unordered_map<string, unordered_map<string, bool> >::iterator relatedElementListIter = _relatedElementList.begin(); relatedElementListIter != _relatedElementList.end(); relatedElementListIter++)
	{
		if (hashList.find(relatedElementListIter->first) != hashList.end())
		{
			int keyHash = hashList[relatedElementListIter->first];
			AddHashToList(keyHash);
			AddRelatedHashPair(keyHash, keyHash);
			for (unordered_map<string, bool>::iterator relatedElementIter = relatedElementListIter->second.begin(); relatedElementIter != relatedElementListIter->second.end(); relatedElementIter++)
			{
				if (hashList.find(relatedElementIter->first) != hashList.end())
				{
					int valueHash = hashList[relatedElementIter->first];
					AddHashToList(valueHash);
					AddRelatedHashPair(keyHash, valueHash);
				}
			}
		}
	}
}

void ResourceFile::AddElementToList(string element)
{
	if (_elementList.find(element) == _elementList.end())
	{
		_elementList[element] = true;
	}
}

void ResourceFile::AddHashToList(int hash)
{
	if (_hashList.find(hash) == _hashList.end())
	{
		_hashList[hash] = true;
	}
}

void ResourceFile::AddRelatedElementPair(string keyElement, string valueElement)
{
	if (_relatedElementList.find(keyElement) == _relatedElementList.end())
	{
		unordered_map<string, bool> relatedElement;
		_relatedElementList[keyElement] = relatedElement;
	}
	_relatedElementList[keyElement][valueElement] = true;
}

void ResourceFile::AddRelatedHashPair(int keyHash, int valueHash)
{
	if (_relatedHashList.find(keyHash) == _relatedHashList.end())
	{
		unordered_map<int, bool> relatedHash;
		_relatedHashList[keyHash] = relatedHash;
	}
	_relatedHashList[keyHash][valueHash] = true;
}