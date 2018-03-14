#include "InputStream.h"

void InputStream::openR()
{
	f = fopen(m_filename.c_str(), "r");
	checkFileOpenError();
}

void InputStream::openRB()
{
	f = fopen(m_filename.c_str(), "rb");
	checkFileOpenError();
}

char InputStream::readChar()
{
	if (!error)
	{
		return (char)fgetc(f);
	}
}

