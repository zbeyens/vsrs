#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "FileStream.h"

class InputStream : public FileStream
{
public:
	InputStream(string filename) : FileStream(filename) {}
	~InputStream() {}

	//! Read the file (read-only)
	void openR();
	void openRB();

	//! Return a read char of the file
	char readChar();

	template <class PixelType>
	void readOneFrame(PixelType* buffer, int size, int frameno);
};

template <class PixelType>
void InputStream::readOneFrame(PixelType* buffer, int size, int frameno)
{
	if (f == NULL) error = true;

	if (frameno != -1)
	{
		long offs = long(size)*long(frameno);
		fseek(f, offs, SEEK_SET);
	}

	if (fread(buffer, size, 1, f) != 1)
	{
		fprintf(stderr, "EOF\n");
		error = true;
	}
}