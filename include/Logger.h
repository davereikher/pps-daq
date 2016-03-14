#pragma once
#include <memory>

class Logger
{
public:
	static Logger& Instance();
	Logger();
	void Init(std::string a_sFilePath, std::string a_sRootFile);
	void NewEntry(int a_iEntryNum);
	void SetWriteCurrentMessage();
	void AddNecessaryMessage(std::string a_sMessage);
	void AddMessage(std::string a_sMessage);
	~Logger();
private:
	void Flush();
private:
	FILE* m_pFile;
	static Logger m_instance;
	std::string m_sCurrentMsg;
	bool m_bWriteCurrentMsg;
	bool m_bIsInit;
	int m_iCurrentEntry;
};
