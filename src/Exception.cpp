#include "Exception.h"

ExceptionBase::ExceptionBase(std::string a_sMsg):
m_sDescription(a_sMsg)
{}

std::string ExceptionBase::What()
{
	return m_sDescription;
}

EventParsingException::EventParsingException(std::string a_sExceptionMessage):
ExceptionBase(a_sExceptionMessage)
{}
