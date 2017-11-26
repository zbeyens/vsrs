#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "ConfigSyn.h"
#include "Singleton.h"


using namespace std;


class Parser : public Singleton<Parser>
{
	friend Singleton<Parser>;

public:
	uint parse();

	void setFileName(string filename);
	void setCommands(int argc, char** argv);

private:
	Parser() {}
	~Parser() {}

	string mFilename;
	vector<char*> mCommands;

	map<string, string> mParams; //!< All the config lines : <tag, value>


	/*!
		Read the config file line by line
		If the tag equals to one of m_pCfgLines tag:
		Set its var to the given value

		\return
			1: Success
			-1: Failed
	*/
	int readSynFile();
	
	/*!
		Read line and store tag and value

		\return
			1: Success
			-1: Failed
	*/
	int readLine(FILE* hFile, string* pacTag);

	int readFromCommandLine();
	int readCommandLine(char *buf, string* pacTag);

	/*!
		\brief
		   Read the camera parameters from file to memory

		Scan the file till reaching the id of one of the 3 camera parameters (given in the config file)
		Scan the values and store them in the 2 matrix and translation vector
		Repeat for the 3 cameras
		
		\return
			1: Succeed
			0: Fail
	*/
	//todo divide the function
	//todo order of camera indexes
	uint readCameraFile();
};