#pragma once
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

class CommonUtils
{
public:

	static std::vector<float>& GenerateTimeSequence(unsigned int a_iNumOfSamples, float a_fSamplingFrequencyGHz = 0);
	static int MkPath(const char *path, mode_t mode);

private:	
	static int DoMkDir(const char *path, mode_t mode);
};
