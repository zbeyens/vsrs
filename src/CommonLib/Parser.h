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
		\return
			VSRS_OK
			VSRS_ERROR
	*/
	int parse();

	void setFilename(string filename);
	void setCommands(int argc, char** argv);

private:
	Parser() {}
	~Parser() {}

	string mFilename;				//!< Filename to parse
	vector<char*> mCommands;		//!< All the command lines

	map<string, string> mParams;	//!< All the parsed config lines : <tag, value>

	/** Parse the config file

		Read the config file line by line with an InputStream
		For each valid line read, stores its tag and value in params map
		\see readLine
		\return
			VSRS_OK
			VSRS_ERROR
	*/
	int readSynFile();

	/** Parse one line of the config file

		Read chars till EOL or EOF
		Stores tag and its value
		Ignore comments

		\return
			VSRS_OK
			VSRS_ERROR
	*/
	void readLine(InputStream istream, string* tagValue);

	/** Parse commands

		For each valid command, stores its tag and value in params map
		\see readCommandLine
	*/
	void readFromCommandLine();

	/** Parse one command

		Stores one command in the format "tag=value"
		Ignore comments
	*/
	void readCommandLine(char *buf, string* tagValue);

	/** Read the camera parameters from file to memory

		Parse all camera
		\see parseOneCamera
		\return
			VSRS_OK
			VSRS_ERROR
	*/
	int readCameraFile();

	/**	Parse one camera

		\see parseCameraId
		\see parseCameraIntrinsics
		\see parseSeparator
		\see parseCameraExtrinsics
		\return
			VSRS_OK
			VSRS_ERROR
	*/
	int parseOneCamera(InputStream istream, const char* cameraId, int cameraIndex);

	/** Scan the camera id

		Scan the id of one of the 3 camera parameters (given in the config file)
		\return
			VSRS_OK
			VSRS_ERROR
	*/
	int parseCameraId(InputStream istream, char* parsedCameraId);

	/** Parse the camera intrinsics
		\return
			number of char read
	*/
	int parseCameraIntrinsics(InputStream istream, int cameraIndex);

	/** Parse separator int-ext
		\return
			number of char read
	*/
	int parseSeparator(InputStream istream);

	/** Parse the camera extrinsics
		\return
			number of char read
	*/
	int parseCameraExtrinsics(InputStream istream, int cameraIndex);
};