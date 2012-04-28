/*
 * CppLogger.h
 *
 *  Created on: 06-04-2012
 *  Author: Adrian Skoczylas
 */

#ifndef CPPLOGGER_H_
#define CPPLOGGER_H_

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <pthread.h>
#include <cstring>
#include <algorithm>
#include <sstream>

class MutexObject;

/**
 * This class allows to log two types events (ERROR and DEBUG).
 * You don`t must create new variables each time you want to log something.
 * This library is fully thread safe.
 */
class CppLogger
{
private:
	static CppLogger *log;
	std::fstream fileDebug, fileError;
	int debugFileSizeMB, errorFileSizeMB;
	bool isDebugLog, isErrorLog;
	std::string directory, filename;
	int filesNum;

public:
	/**
	 * Function to log debug event
	 */
    static void logDebug(const char*);
    /**
	 * Function to log error event. Error events are also log to debug file.
	 */
    static void logError(const char*);

private:
	CppLogger();
	virtual ~CppLogger();

	static CppLogger *getInstance();
	void error(const char*);
	void debug(const char*, bool);
	void parseConfFile();
	std::string getVar(std::ifstream&);
};



#endif /* CPPLOGGER_H_ */
