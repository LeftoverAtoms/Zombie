#include <iostream>

#include "Log.h"

const char* FilePath = ".\\logs\\";
const char* FileName = "output.log";
char* ActualPath;

std::ofstream Log::Output;

void Log::Init()
{
	// TODO: Numbered logs to not override
}

void Log::Info(const char* message)
{
	{
#ifdef _DEBUG
		std::cout << "Info: " << message << '\n';
#endif

		Output.open(FilePath, std::ios_base::app);
		Output << "Info: " << message << '\n';
		Output.close();
	}
}
void Log::Warning(const char* message)
{
	{
#ifdef _DEBUG
		std::cout << "Warning: " << message << '\n';
#endif

		Output.open(FilePath, std::ios_base::app);
		Output << "Warning: " << message << '\n';
		Output.close();
	}
}
void Log::Error(const char* message)
{
	{
#ifdef _DEBUG
		std::cout << "Error: " << message << '\n';
#endif

		Output.open(FilePath, std::ios_base::app);
		Output << "Error: " << message << '\n';
		Output.close();
	}
}