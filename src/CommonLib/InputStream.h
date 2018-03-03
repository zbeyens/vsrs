#pragma once

#include "SystemIncludes.h"

class InputStream
{
public:
	InputStream(const char* filename);
	~InputStream() {}

	//! Read the file (read-only)
	void startReading();

	//! Close the file
	void endReading();

	//! Seek to the given offset in the file
	void seek(int offset);

	//! Return a read char of the file
	char readChar();

	//! Return whether the file is EOF
	int isEndOfFile();

	FILE* getFile();
	bool hasError();

private:
	FILE* f;				//!< File object
	const char* mFilename;	//!< Filename
	bool error = false;		//!< Error if open failed
};