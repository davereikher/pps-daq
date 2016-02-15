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

class ExceptionWithStrMsg: public ExceptionBase
{
public:
	ExceptionWithStrMsg(int a_iLine, std::string a_sMsg);
};

class EventParsingException: public ExceptionBase
{
public:
	EventParsingException(int a_iSize, int a_iOffset, int a_iLine);
};

class EventHandlerException: public ExceptionWithStrMsg
{
public:
	EventHandlerException(int a_iLine, std::string a_sMsg);
};

class InitException: public ExceptionWithStrMsg
{
public:
	InitException(int a_iLine, std::string a_sMsg, int a_iErrorCode);
};
