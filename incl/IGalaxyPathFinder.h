#pragma once

class IGalaxyPathFinder 
{
public:
	virtual void FindSolution(const char* inputJasonFile, const char* outputFileName) = 0;
	virtual const char* ShowCaptainName() = 0;
};
