#pragma once

#include "SystemIncludes.h"
#include "Singleton.h"


#define BYTE unsigned char
#define WORD unsigned short

#define uint unsigned int
typedef unsigned char uchar;



class ConfigBase : public Singleton<ConfigBase>
{
	friend Singleton<ConfigBase>;

public:
	//Parser


private:
	ConfigBase() {};
	~ConfigBase() {};
};