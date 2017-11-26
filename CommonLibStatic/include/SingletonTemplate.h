#pragma once

#include <iostream>
#include <string>
#include "Singleton.h"

using namespace std;


class Parser : public Singleton<Parser>
{
	friend Singleton<Parser>;

public:


private:
	Parser() {};
	~Parser() {};

}