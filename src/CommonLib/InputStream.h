#pragma once

#include "SystemIncludes.h"
#include "ConfigSyn.h"
#include "FileStream.h"

class InputStream : public FileStream
{
public:
	InputStream(string filename) : FileStream(filename) {}
	~InputStream() {}

	/*!
		Open the file (read-only)
	*/
	void openR();
	/*!
		Open the file (binary read-only)
	*/
	void openRB();

	/*!
		Read a char of the file
		@return	the read char
	*/
	char readChar();

	/*!
		Read one frame of an image or video and stores it into the buffer

		@param buffer	where to store the read characters
		@param size		the size of the frame
		@param frameno	the number of the frame to read
	*/
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