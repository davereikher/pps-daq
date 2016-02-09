#include <stdio.h>
#include "Exception.h"

#define EXCEPTION_MESSAGE_MAX_LEN 200

ExceptionBase::ExceptionBase()
{}

ExceptionBase::ExceptionBase(std::string a_sMsg):
m_sDescription(a_sMsg)
{}

std::string ExceptionBase::What()
{
	return m_sDescription;
}

EventParsingException::EventParsingException(int a_iSize, int a_iOffset)
{
	char pMsg[EXCEPTION_MESSAGE_MAX_LEN] = {0};
	
	//not much to do on error here
	snprintf(pMsg, EXCEPTION_MESSAGE_MAX_LEN, "Parsing error: tried to parse offset %d but exceeded buffer size (%d)", a_iSize, a_iOffset);
	m_sDescription = pMsg;
}
