#include "MethodInfo.h"

MethodInfo::MethodInfo()
{
}

MethodInfo::MethodInfo(Parameter* userSetting, int threadID, VocabTable* vocabTable)
{
	_userSetting = userSetting;
	_threadID = threadID;
	_vocabTable = vocabTable;
}

void MethodInfo::SetRCMResource(ResourceFile* rcmResource)
{
	_rcmResource = rcmResource;
}