#include <iostream>

static struct Log
{
	static inline void Info(const char* message)
	{
#ifdef _DEBUG
		std::cout << "Info: " << message << '\n';
#endif
	}

	static inline void Warning(const char* message)
	{
#ifdef _DEBUG
		std::cout << "Warning: " << message << '\n';
#endif
	}

	static inline void Error(const char* message)
	{
#ifdef _DEBUG
		std::cout << "Error: " << message << '\n';
#endif
	}
};