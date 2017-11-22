//======================================created by Ying Chen =====================================
//===============Tampere University of Technology (TUT)/Nokia Research Center (NRC)===============

#ifndef AFX_PARAMETER_BASE_H
#define AFX_PARAMETER_BASE_H

#include "iostream"
#include "string"

#define UInt    unsigned int
#define Bool    bool
#define Double  double 
#define Int     int
#define Char    char
#define Void    void

#define MAX_CONFIG_PARAMS 256

//using namespace std ;

class ConfigLineBase
{
protected:
	//! Init m_cTag, m_uiType
	ConfigLineBase(Char* pcTag, UInt uiType) : m_cTag(pcTag), m_uiType(uiType) {}
	ConfigLineBase() {}

public:
	virtual ~ConfigLineBase() {}
	std::string&  getTag() { return m_cTag; }
	virtual Void  setVar(std::string& rcValue) = 0;
	virtual Void  fprintVar(FILE *fp) = 0;

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
	UInt m_uiType;
};

//! ConfigLineStr - string
class ConfigLineStr : public ConfigLineBase
{
public:
	/*
		Call ConfigLineBase
		Set parameter to default value
	*/
	ConfigLineStr(Char* pcTag, std::string* pcPar, Char* pcDefault);
	Void setVar(std::string& pvValue); //!< Set param to a new value
	Void fprintVar(FILE *fp);

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
	ConfigLineDbl(Char* pcTag, Double* pdPar, Double pdDefault);
	Void setVar(std::string& pvValue); //!< Set param to a new value (converted to double)
	Void fprintVar(FILE *fp);

protected:
	Double* m_pdPar;
};

//! ConfigLineInt - int
class ConfigLineInt : public ConfigLineBase
{
public:
	/*
		Call ConfigLineBase
		Set parameter to default value
	*/
	ConfigLineInt(Char* pcTag, Int* piPar, Int piDefault);
	Void setVar(std::string& pvValue); //!< Set param to a new value (converted to int)
	Void fprintVar(FILE *fp);

protected:
	Int* m_piPar;
};

//! ConfigLineUInt - uint
class ConfigLineUInt : public ConfigLineBase
{
public:
	/*
		Call ConfigLineBase
		Set puiPar to puiDefault value
	*/
	ConfigLineUInt(Char* pcTag, UInt* puiPar, UInt puiDefault);
	
	Void setVar(std::string& pvValue); //!< Set param to a new value (converted to uint)
	Void fprintVar(FILE *fp);

protected:
	UInt* m_puiPar;
};

//! ConfigLineChar - char
class ConfigLineChar : public ConfigLineBase
{
public:
	/*
		Call ConfigLineBase
		Set parameter to default value
	*/
	ConfigLineChar(Char* pcTag, Char* pcPar, Char pcDefault);
	Void setVar(std::string& pvValue);	//!< Set param to a new value (converted to char)
	Void fprintVar(FILE *fp);			//!< Print the "tag parameter : value"

protected:
	Char* m_pcPar; //!< Parameter reference from CParameterViewInterpolation
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

	virtual Int  Init(Int argc, Char** argv) = 0;

	/*!
		Read line and store tag and value

		\return
		1: Success
		-1: Failed
	*/
	Int          xReadLine(FILE* hFile, std::string* pacTag);

	/*!
		Read the config file line by line
		If the tag equals to one of m_pCfgLines tag:
		Set its var to the given value

		\return
		1: Success
		-1: Failed
	*/
	Int          xReadFromFile(std::string& rcFilename);

	Int          xReadFromCommandLine(Int argc, Char **argv);
	Int          xReadCommandLine(char *buf, std::string* pacTag);


	Void         xPrintParam(); //!< Print all parameters

protected:
	virtual UInt setup() = 0;

	Void         release();	//!< Delete all objects from m_pCfgLines

protected:
	ConfigLineBase*  m_pCfgLines[MAX_CONFIG_PARAMS]; //!< Stores all the possible ConfigLineBase followed by NULL
};

#endif 
