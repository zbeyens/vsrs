#pragma once

#include "SystemIncludes.h"
#include "Singleton.h"


class CameraManager : public Singleton<CameraManager>
{
	friend Singleton<CameraManager>;

public:
	void add();

private:
	CameraManager() {};
	~CameraManager() {};

};