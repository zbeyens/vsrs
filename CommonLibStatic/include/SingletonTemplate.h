#pragma once

#include "SystemIncludes.h"
#include "Singleton.h"


class Template : public Singleton<Template>
{
	friend Singleton<Template>;

public:


private:
	Template() {};
	~Template() {};

}