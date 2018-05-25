#pragma once

#include "SystemIncludes.h"
#include "Singleton.h"
#include "ConfigSyn.h"
#include "InputStream.h"

class Parser : public Singleton<Parser>
{
	friend Singleton<Parser>;

public:
	/**	Parse config and camera files

		Parse config file and command line
		Set ConfigSyn parameters
		Parse camera file
		Set ConfigCam parameters
		\see readSynFile
		\see readFromCommandLine
		\see readCameraFile
	*/
	bool parse();

	void setFilename(string filename);
	void setCommands(int argc, char** argv);

private:
	Parser() {}
	~Parser() {}

	string m_filename;				//!< Filename to parse
	vector<char*> m_commands;		//!< All the command lines

	map<string, string> m_params;	//!< All the parsed config lines : <tag, value>

	/** Parse the config file

		Read the config file line by line with an InputStream
		For each valid line read, stores its tag and value in params map
	*/
	bool parseConfigFile();

	/** Parse one line of the config file

		Read chars till EOL or EOF
		Stores tag and its value
		Ignore comments
	*/
	void parseLine(InputStream istream, string* tagValue);

	/** Parse commands

		For each valid command, stores its tag and value in params map
		\see readCommandLine
	*/
	void parseCommandLine();

	/** Parse one command

		Stores one command in the format "tag=value"
		Ignore comments
	*/
	void readCommandLine(char *buf, string* tagValue);

	/** Read the camera parameters from file to memory

		Parse all camera
		\see parseOneCamera
	*/
	bool parseCameraFile();

	/**	Parse one camera

		Find the camera id to parse the intrinsic and extrinsic parameters that are separated by two "0.0".

		\see parseCameraId
		\see parseCameraIntrinsics
		\see parseSeparator
		\see parseCameraExtrinsics
	*/
	bool parseOneCamera(InputStream istream, const char* cameraId, int cameraIndex);

	/** Scan the camera id

		Scan the id of one of the camera parameters (given in the config file)
	*/
	int parseCameraId(InputStream istream, char* parsedCameraId);

	/** Parse the camera intrinsics
		\return
			number of char read
	*/
	int parseCameraIntrinsics(InputStream istream, ConfigCam* configCam);

	/** Parse separator int-ext
		\return
			number of char read
	*/
	int parseSeparator(InputStream istream);

	/** Parse the camera extrinsics
		\return
			number of char read
	*/
	int parseCameraExtrinsics(InputStream istream, ConfigCam* configCam);
};