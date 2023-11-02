#pragma once

#include <fstream>

static struct Log
{
	static std::ofstream Output;

	static void Init();

	static void Info(const char* message);
	static void Warning(const char* message);
	static void Error(const char* message);
};