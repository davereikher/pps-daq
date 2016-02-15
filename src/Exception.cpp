#include <stdio.h>
#include "Exception.h"

#define EXCEPTION_MESSAGE_MAX_LEN 400

ExceptionBase::ExceptionBase()
{}

ExceptionBase::ExceptionBase(std::string a_sMsg):
m_sDescription(a_sMsg)
{}

std::string ExceptionBase::What()
{
	return m_sDescription;
}

ExceptionWithStrMsg::ExceptionWithStrMsg(int m_iLine, std::string a_sMsg)
{
	char pMsg[EXCEPTION_MESSAGE_MAX_LEN] = {0};
	snprintf(pMsg, EXCEPTION_MESSAGE_MAX_LEN, "ERROR (line %d): ", m_iLine);
	
	m_sDescription = pMsg;
	m_sDescription += a_sMsg;	
}

EventParsingException::EventParsingException(int a_iSize, int a_iOffset, int a_iLine)
{
	char pMsg[EXCEPTION_MESSAGE_MAX_LEN] = {0};
	
	//not much to do on error here
	snprintf(pMsg, EXCEPTION_MESSAGE_MAX_LEN, "Parsing error: tried to parse offset %d but exceeded buffer size (%d), around line %d", a_iOffset, a_iSize, a_iLine);
	m_sDescription = pMsg;
}

EventHandlerException::EventHandlerException(int a_iLine, std::string a_sMsg):
ExceptionWithStrMsg(a_iLine, a_sMsg)
{}

InitException::InitException(int a_iLine, std::string a_sMsg, int a_iErrorCode):
ExceptionWithStrMsg(a_iLine, a_sMsg)
{
	char pMsg[EXCEPTION_MESSAGE_MAX_LEN] = {0};
	snprintf(pMsg, EXCEPTION_MESSAGE_MAX_LEN, ". Error code: %d: ", a_iErrorCode);
	m_sDescription += pMsg;
}

