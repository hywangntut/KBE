#include "ICFile.h"

ICFile::ICFile()
{
}

ICFile::ICFile(string icFilePath)
{
	ifstream icFile(icFilePath.c_str());
	char buffer[BUFFER_SIZE] = " ";
	string keyElement = "", valueElement = "";

	_fileExist = icFile.good();
	if (_fileExist)
	{
		while (strcmp(buffer, ""))
		{
			icFile.getline(buffer, BUFFER_SIZE, '\n');
			istringstream relatedElementPair(buffer);
			relatedElementPair >> keyElement;
			if (keyElement != "")
			{
				relatedElementPair >> valueElement;
				if (valueElement != "")
				{
					double informationContent = stod(valueElement);
					_informationContent[keyElement] = informationContent;
				}
			}
		}
	}
	icFile.close();
}

bool ICFile::IsExist()
{
	return _fileExist;
}

double ICFile::GetInformationContent(string word)
{
	if (_informationContent.find(word) != _informationContent.end())
		return _informationContent[word] + 0.000001;
	return 0.000001;
}