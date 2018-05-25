#pragma once

#include "SystemIncludes.h"
#include "FileStream.h"

class OutputStream : public FileStream
{
public:
	OutputStream(string filename) : FileStream(filename) {}
	~OutputStream() {}

	/**
		Open the file (binary write-only)
	*/
	void openWB();

	/**
		Write one frame from the buffer

		@param buffer	the buffer containing the characters to write
		@param size		the size of the frame
	*/
	template<class PixelType>
	void writeOneFrame(PixelType* buffer, int size);
};

template<class PixelType>
void OutputStream::writeOneFrame(PixelType* buffer, int size)
{
	if (f == NULL) error = true;

	if (fwrite(buffer, size, 1, f) != 1)
	{
		fprintf(stderr, "EOF\n");
		error = true;
	}
}