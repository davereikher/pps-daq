#pragma once
#include <string>

class ExceptionBase
{
public:
	ExceptionBase();
	ExceptionBase(std::string a_sMsg);
	virtual std::string What();

protected:
	std::string m_sDescription;
};

class EventParsingException: public ExceptionBase
{
public:
	EventParsingException(int a_iSize, int a_iOffset, int a_iLine);
};

