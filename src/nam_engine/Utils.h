#pragma once
#include <ContainerSequential.h>
#include <string>

class Utils
{
public:
	static std::string GetASCIIstring(int c);

	static std::string GetFloatDisplay(float val, int decimals);
};