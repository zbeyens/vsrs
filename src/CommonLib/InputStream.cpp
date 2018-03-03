#include "InputStream.h"

InputStream::InputStream(const char * filename)
{
	mFilename = filename;
}

void InputStream::startReading()
{
	f = fopen(mFilename, "r");
	if (f == NULL)
	{
		error = true;
		cout << "Failed to open " << mFilename << " file" << endl;
	}
}
void InputStream::endReading()
{
	if (error) return;

	fclose(f);
}

void InputStream::seek(int offset)
{
	int err = fseek(f, offset, SEEK_SET);
	if (err) error = true;
}

char InputStream::readChar()
{
	if (!error)
	{
		return (char)fgetc(f);
	}
}

int InputStream::isEndOfFile()
{
	return feof(f);
}

FILE * InputStream::getFile()
{
	return f;
}

bool InputStream::hasError()
{
	return error;
}