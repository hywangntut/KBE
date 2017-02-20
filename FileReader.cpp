#include "FileReader.h"

void FileReader::ReadElement(FILE *file, char *element)
{
	int i = 0, wordChar;
	while (!feof(file))
	{
		wordChar = fgetc(file);
		if (wordChar == 13)
		{
			continue;
		}
		if ((wordChar == ' ') || (wordChar == '\t') || (wordChar == '\n'))
		{
			if (i > 0)
			{
				if (wordChar == '\n')
				{
					ungetc(wordChar, file);
				}
				break;
			}
			if (wordChar == '\n')
			{
				strcpy(element, (char *)"</s>");
				return;
			}
			else
			{
				continue;
			}
		}
		element[i] = wordChar;
		i++;
		if (i >= MAX_WORD_LENGTH - 1)
		{
			i--;
		}
	}
	element[i] = 0;
}

int FileReader::ReadElementHash(FILE *file, VocabTable* vocabTable)
{
	char element[MAX_WORD_LENGTH];
	ReadElement(file, element);
	if (feof(file))
	{
		return -1;
	}
	return vocabTable->SearchVocab(element);
}