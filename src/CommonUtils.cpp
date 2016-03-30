#include "CommonUtils.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
typedef struct stat Stat;

/**
Creates a time sequence for the time axis, for plotting, based on the provided sampling frequency and number of samples

@param a_iNumOfSamples: number of samples
@param a_iSamplingFrequencyGHz sampling frequency in GHz. If it's 0, the result will be just a sequence (0 to a_iNumOfSamples). Default value is 0.
@return a vector of time values, in nanoseconds
*/
std::vector<float>& CommonUtils::GenerateTimeSequence(unsigned int a_iNumOfSamples, float a_fSamplingFrequencyGHz)
{
	static std::vector<float> vTimeSeq;
	static int iLastNumOfsamples = -1;
	static float fLastSamplingFrequencyGHz = -1;

	if((a_iNumOfSamples == iLastNumOfsamples) && (a_fSamplingFrequencyGHz == fLastSamplingFrequencyGHz))
	{
		return vTimeSeq;
	}

	iLastNumOfsamples = a_iNumOfSamples;
	fLastSamplingFrequencyGHz = a_fSamplingFrequencyGHz;

	float fTimeStep = 1.0/a_fSamplingFrequencyGHz;
	
	vTimeSeq.resize(a_iNumOfSamples);
	for (int i = 0; i < (int)a_iNumOfSamples; i++)
	{
		vTimeSeq[i] = i * fTimeStep;
	}

	return vTimeSeq;
}

int CommonUtils::DoMkDir(const char *path, mode_t mode)
{
    Stat            st;
    int             status = 0;

    if (stat(path, &st) != 0)
    {
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir(path, mode) != 0 && errno != EEXIST)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }

    return(status);
}

/**
** mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists, rather than optimistically creating
** the last element and working backwards.
*/
int CommonUtils::MkPath(const char *path, mode_t mode)
{
    char           *pp;
    char           *sp;
    int             status;
    char           *copypath = strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = DoMkDir(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0)
        status = DoMkDir(path, mode);
    free(copypath);
    return (status);
}
