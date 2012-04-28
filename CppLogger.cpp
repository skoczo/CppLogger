/*
 * CppLogger.cpp
 *
 *  Created on: 06-04-2012
 *  Author: Adrian Skoczylas
 */

#include "CppLogger.h"

CppLogger *CppLogger::log = 0;

/**
 * Object need to provide thread safe.
 */
class MutexObject
{
	static pthread_mutex_t mutex;
public:
	MutexObject()
	{
		pthread_mutex_lock(&mutex);
	}

	~MutexObject()
	{
		pthread_mutex_unlock(&mutex);
	}
};

pthread_mutex_t MutexObject::mutex = PTHREAD_MUTEX_INITIALIZER;

CppLogger::CppLogger()
{
	//start set default values
	isDebugLog = false;
	isErrorLog = false;
	debugFileSizeMB = 10;
	errorFileSizeMB = 10;
	directory = "logs";
	filename = "log";
	filesNum = 2;
	//end set default values

	//try to get settings from file
	parseConfFile();

	struct stat st;
	//check if directory exist
	if (stat(directory.c_str(), &st) != 0)
	{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		std::string command = "mkdir ";
		command+= directory;
		system(command.c_str());
#else
		//if not exist create directory
		mkdir(directory.c_str(), 0777);
#endif
	}

	fileDebug.open((directory + '/' + filename + "_debug.txt").c_str(),
			std::ios::out | std::ios::in | std::ios::app);
	fileError.open((directory + '/' + filename + "_error.txt").c_str(),
			std::ios::out | std::ios::in | std::ios::app);
}

CppLogger *CppLogger::getInstance()
{
	MutexObject mutex;
	if (log == 0)
	{
		log = new CppLogger();
	}

	return log;
}

void CppLogger::debug(const char* c, bool isError)
{
	MutexObject mutex;
	long begin, end;
	fileDebug.seekg(0, std::ios::beg);
	begin = fileDebug.tellg();
	fileDebug.seekg(0, std::ios::end);
	end = fileDebug.tellg();

	long size = end - begin;

	if (size > debugFileSizeMB * 1024 * 1024)
	{
		fileDebug.close();

		for (int i = filesNum - 1; i >= 0; i--)
		{
			std::stringstream ss;
			if (i - 1 > 0)
				ss << i;

			std::string file1 = directory + '/' + filename + "_debug.txt"
					+ ss.str();

			ss.str("");
			if (i - 1 > 0)
				ss << i - 1;

			std::string file2 = directory + '/' + filename + "_debug.txt"
					+ ss.str();

			if (i == filesNum - 1)
			{
				std::ifstream f;
				f.open((file1).c_str());
				if (f.is_open())
				{
					f.close();
					remove((file1).c_str());
				}
			}
			if (filesNum != 1)
				rename(file2.c_str(), file1.c_str());
		}
		fileDebug.open((directory + '/' + filename + "_debug.txt").c_str(),
				std::ios::out | std::ios::in | std::ios::app);
	}

	if (!fileDebug.is_open())
	{
		std::cout << "debug file is not open" << std::endl;
		return;
	}

	time_t rawtime;

	time(&rawtime);
	time_t t = time(0); // get time now
	struct tm * now = localtime(&t);

	char* type;
	if (isError)
	{
		type = (char*) "[ERROR] ";
	}
	else
	{
		type = (char*) "[DEBUG] ";
	}

	fileDebug << type << (now->tm_year + 1900) << '-' << (now->tm_mon + 1)
			<< '-' << now->tm_mday << " " << now->tm_hour << ":" << now->tm_min
			<< ":" << now->tm_sec << ": ";
	fileDebug << c << std::endl;
}

void CppLogger::error(const char *c)
{
	MutexObject mutex;
	long begin, end;
	fileError.seekg(0, std::ios::beg);
	begin = fileError.tellg();
	fileError.seekg(0, std::ios::end);
	end = fileError.tellg();

	long size = end - begin;

	if (size > errorFileSizeMB * 1024 * 1024)
	{
		fileError.close();

		for (int i = filesNum - 1; i >= 0; i--)
		{
			std::stringstream ss;
			if (i - 1 > 0)
				ss << i;

			std::string file1 = directory + '/' + filename + "_error.txt"
					+ ss.str();

			ss.str("");
			if (i - 1 > 0)
				ss << i - 1;

			std::string file2 = directory + '/' + filename + "_error.txt"
					+ ss.str();

			if (i == filesNum - 1)
			{
				std::ifstream f;
				f.open((file1).c_str());
				if (f.is_open())
				{
					f.close();
					remove((file1).c_str());
				}
			}
			if (filesNum != 1)
				rename(file2.c_str(), file1.c_str());
		}
		fileError.open((directory + '/' + filename + "_error.txt").c_str(),
				std::ios::out | std::ios::in | std::ios::app);
	}

	if (!fileError.is_open())
	{
		std::cout << "error file is not open" << std::endl;
		return;
	}

	time_t rawtime;

	time(&rawtime);
	time_t t = time(0); // get time now
	struct tm * now = localtime(&t);

	fileError << "[ERROR] " << (now->tm_year + 1900) << '-'
			<< (now->tm_mon + 1) << '-' << now->tm_mday << " " << now->tm_hour
			<< ":" << now->tm_min << ":" << now->tm_sec << ": ";
	fileError << c << std::endl;
}

CppLogger::~CppLogger()
{
	std::cout << "close" << std::endl;
	fileDebug.close();
	fileError.close();
}

void CppLogger::logDebug(const char* c)
{
	if (getInstance()->isDebugLog)
		CppLogger::getInstance()->debug(c, false);
}

void CppLogger::logError(const char* c)
{
	if (getInstance()->isErrorLog)
	{
		CppLogger::getInstance()->error(c);

		if (getInstance()->isDebugLog)
			CppLogger::getInstance()->debug(c, true);
	}
}

void CppLogger::parseConfFile()
{
	std::ifstream file;
	file.open("CppLogger.conf");
	if (file.is_open())
	{
		while (file.good())
		{
			std::string type = getVar(file);

			if (type == "isDebugLog")
			{
				std::string var = getVar(file);
				std::transform(var.begin(), var.end(), var.begin(), ::tolower);
				if (var == "true")
				{
					isDebugLog = true;
				}
			}
			else if (type == "isErrorLog")
			{
				std::string var = getVar(file);
				std::transform(var.begin(), var.end(), var.begin(), ::tolower);
				if (var == "true")
				{
					isErrorLog = true;
				}
			}
			else if (type == "filesNum")
			{
				std::string var = getVar(file);
				int number = atoi(var.c_str());
				if (number > 0)
				{
					filesNum = number;
				}
			}
			else if (type == "filename")
			{
				std::string var = getVar(file);
				if (var.size() > 0)
				{
					filename = var;
				}
			}
			else if (type == "directory")
			{
				std::string var = getVar(file);
				if (var.size() > 0)
				{
					directory = var;
				}
			}
		}
	}
}

std::string CppLogger::getVar(std::ifstream& file)
{
	std::string result = "";

	while (file.good())
	{
		char c = file.get();
		if (c == '=' || c == '\n')
		{
			break;
		}
		else if (c == '#')
		{
			while (file.good() && file.get() != '\n')
			{
			}
			break;
		}
		else if (c == ' ')
		{
			continue;
		}

		result += c;
	}

	return result;
}
