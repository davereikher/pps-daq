#pragma once
#include <string>

class ExceptionBase
{
public:
	ExceptionBase(std::string a_sMsg);
	virtual std::string What();

private:
	std::string m_sDescription;
};

class EventParsingException: public ExceptionBase
{
public:
	EventParsingException(std::string a_sExceptionMessage);
};

