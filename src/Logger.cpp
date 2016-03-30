#include <errno.h>
#include "Logger.h"

Logger Logger::m_instance;

Logger & Logger::Instance()
{
	return m_instance;
}

Logger::Logger():
m_pFile(NULL),
m_bWriteCurrentMsg(false),
m_bIsInit(false),
m_iCurrentEntry(0)
{
}

void Logger::Init(std::string a_sFilePath)
{
	m_pFile = fopen(a_sFilePath.c_str(), "w");
	if (NULL == m_pFile)
	{
		printf("Could not open log file %s for writing! Error = %d \n", a_sFilePath.c_str(), errno);
		//TODO: exception?
		exit(1);
	}
	std::string sInitialMsg = std::string("PPS log") + "\n\n";
	fwrite(sInitialMsg.c_str(), sInitialMsg.size(), 1, m_pFile);
	m_bIsInit = true;
}

void Logger::AddNecessaryMessage(std::string a_sMessage)
{
	if(!m_bIsInit)
	{
		return;
	}

	fwrite(a_sMessage.c_str(), a_sMessage.size(), 1, m_pFile);
}

void Logger::NewEntry(int a_iEntryNum)
{
	if(!m_bIsInit)
	{
		return;
	}

	Flush();
	m_iCurrentEntry = a_iEntryNum;
	m_bWriteCurrentMsg = false;
	m_sCurrentMsg = "";
}

void Logger::AddMessage(std::string a_sMsg)
{
	if(!m_bIsInit)
	{
		return;
	}
	m_sCurrentMsg += "\t" + a_sMsg + "\n";
}

void Logger::SetWriteCurrentMessage()
{
	m_bWriteCurrentMsg = true;
}

void Logger::Flush()
{
	if (m_bWriteCurrentMsg)
	{
		m_sCurrentMsg = "Entry No. " + std::to_string(m_iCurrentEntry) + "\n" + m_sCurrentMsg;
		fwrite(m_sCurrentMsg.c_str(), m_sCurrentMsg.size(), 1, m_pFile);
		fflush(m_pFile);
	}
}

Logger::~Logger()
{
	if(!m_bIsInit)
	{
		return;
	}
	Flush();	
	fclose(m_pFile);
}
