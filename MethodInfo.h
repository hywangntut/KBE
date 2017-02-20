#ifndef KNOWLEDGEBASEEMBEDDING_METHOD_METHODINFO_H
#define KNOWLEDGEBASEEMBEDDING_METHOD_METHODINFO_H

#include "Parameter.h"
#include "ResourceFile.h"
#include "VocabTable.h"

class MethodInfo
{
public:
	MethodInfo();
	MethodInfo(Parameter* userSetting, int threadID, VocabTable* vocabTable);
	Parameter* GetUserSetting() const { return _userSetting; };
	int GetThreadID() const { return _threadID; };
	VocabTable* GetVocabTable() const { return _vocabTable; };
	ResourceFile* GetRCMResource() const { return _rcmResource; };
	void SetRCMResource(ResourceFile* rcmResource);

protected:
	Parameter* _userSetting;
	int _threadID;
	VocabTable* _vocabTable;
	ResourceFile* _rcmResource;
};

#endif