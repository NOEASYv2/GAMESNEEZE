#pragma once

#if DEV_MODE
	#define LOG(x) UTILS::Log(x)
#else
	#define LOG(x)
#endif

namespace UTILS
{
	void Log(float message);

	void Log(std::string message);
}