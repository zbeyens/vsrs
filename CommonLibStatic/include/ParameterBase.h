//======================================created by Ying Chen =====================================
//===============Tampere University of Technology (TUT)/Nokia Research Center (NRC)===============

#ifndef AFX_PARAMETER_BASE_H
#define AFX_PARAMETER_BASE_H

#include "iostream"
#include "string"

#define uint    unsigned int
#define bool    bool
#define double  double 
#define int     int
#define char    char
#define void    void

#define MAX_CONFIG_PARAMS 256

//using namespace std ;

class ConfigLineBase
{
protected:
	//! Init m_cTag, m_uiType
	ConfigLineBase(char* pcTag, uint uiType) : m_cTag(pcTag), m_uiType(uiType) {}
	ConfigLineBase() {}

public:
	virtual ~ConfigLineBase() {}
	std::string&  getTag() { return m_cTag; }
	virtual void  setVar(std::string& rcValue) = 0;
	virtual void  fprintVar(FILE *fp) = 0;

protected:
	std::string m_cTag; //!< Parameter tag
	/*!
		Type of the parameter
		1: string
		2: double
		3: int
		4: uint
		5: char
	*/
	uint m_uiType;
};

//! ConfigLineStr - string
class ConfigLineStr : public ConfigLineBase
{
public:
	/*
		Call ConfigLineBase
		Set parameter to default value
	*/
	ConfigLineStr(char* pcTag, std::string* pcPar, char* pcDefault);
	void setVar(std::string& pvValue); //!< Set param to a new value
	void fprintVar(FILE *fp);

protected:
	std::string* m_pcPar;
};

//! ConfigLineDbl - double
class ConfigLineDbl : public ConfigLineBase
{
public:
	/*
		Call ConfigLineBase
		Set parameter to default value
	*/
	ConfigLineDbl(char* pcTag, double* pdPar, double pdDefault);
	void setVar(std::string& pvValue); //!< Set param to a new value (converted to double)
	void fprintVar(FILE *fp);

protected:
	double* m_pdPar;
};

//! ConfigLineInt - int
class ConfigLineInt : public ConfigLineBase
{
public:
	/*
		Call ConfigLineBase
		Set parameter to default value
	*/
	ConfigLineInt(char* pcTag, int* piPar, int piDefault);
	void setVar(std::string& pvValue); //!< Set param to a new value (converted to int)
	void fprintVar(FILE *fp);

protected:
	int* m_piPar;
};

//! ConfigLineuint - uint
class ConfigLineuint : public ConfigLineBase
{
public:
	/*
		Call ConfigLineBase
		Set puiPar to puiDefault value
	*/
	ConfigLineuint(char* pcTag, uint* puiPar, uint puiDefault);
	
	void setVar(std::string& pvValue); //!< Set param to a new value (converted to uint)
	void fprintVar(FILE *fp);

protected:
	uint* m_puiPar;
};

//! ConfigLinechar - char
class ConfigLinechar : public ConfigLineBase
{
public:
	/*
		Call ConfigLineBase
		Set parameter to default value
	*/
	ConfigLinechar(char* pcTag, char* pcPar, char pcDefault);
	void setVar(std::string& pvValue);	//!< Set param to a new value (converted to char)
	void fprintVar(FILE *fp);			//!< Print the "tag parameter : value"

protected:
	char* m_pcPar; //!< Parameter reference from CParameterViewInterpolation
};

//! ParameterBase
class ParameterBase
{
public:
	/*!
		Set all m_pCfgLines values to NULL
	*/
	ParameterBase();
	virtual  ~ParameterBase() {};

	virtual int  Init(int argc, char** argv) = 0;

	/*!
		Read line and store tag and value

		\return
		1: Success
		-1: Failed
	*/
	int          xReadLine(FILE* hFile, std::string* pacTag);

	/*!
		Read the config file line by line
		If the tag equals to one of m_pCfgLines tag:
		Set its var to the given value

		\return
		1: Success
		-1: Failed
	*/
	int          xReadFromFile(std::string& rcFilename);

	int          xReadFromCommandLine(int argc, char **argv);
	int          xReadCommandLine(char *buf, std::string* pacTag);


	void         xPrintParam(); //!< Print all parameters

protected:
	virtual uint setup() = 0;

	void         release();	//!< Delete all objects from m_pCfgLines

protected:
	ConfigLineBase*  m_pCfgLines[MAX_CONFIG_PARAMS]; //!< Stores all the possible ConfigLineBase followed by NULL
};

#endif 
