#include "OutputStream.h"

void OutputStream::openWB()
{
	f = fopen(m_filename.c_str(), "wb");
	checkFileOpenError();
}



