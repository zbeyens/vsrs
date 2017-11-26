#pragma once

#include <iostream>
#include <string>
#include "Singleton.h"

using namespace std;

#define uint    unsigned int


class ConfigBase : public Singleton<ConfigBase>
{
	friend Singleton<ConfigBase>;

public:
	//Parser
	static const int tMAX_CONFIG_PARAMS = 256;


private:
	ConfigBase() {};
	~ConfigBase() {};


};