#pragma once

#include "SystemIncludes.h"

class FileStream
{
public:
	FileStream(string filename);
	~FileStream() {}

	/*!
		Close the file
	*/
	void close();

	/*!
		Seek to the given offset in the file
	*/
	void seek(int offset);

	/*!
		Return whether the file is EOF
	*/
	int isEndOfFile();

	FILE* getFile();
	bool hasError();

protected:
	void checkFileOpenError();

	FILE* f;				//!< File object
	string m_filename;	//!< Filename
	bool error = false;		//!< Error if open failed
};