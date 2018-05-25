#include "FileStream.h"

FileStream::FileStream(string filename)
{
	m_filename = filename;
}


void FileStream::close()
{
	if (error) return;

	fclose(f);
}

void FileStream::seek(int offset)
{
	int err = fseek(f, offset, SEEK_SET);
	if (err) error = true;
}

int FileStream::isEndOfFile()
{
	return feof(f);
}

FILE * FileStream::getFile()
{
	return f;
}

bool FileStream::hasError()
{
	return error;
}

void FileStream::checkFileOpenError()
{
	if (f == NULL)
	{
		error = true;
		cout << "Failed to open the image " << m_filename << " file" << endl;
	}
}