#include <iostream>
#include "CppLogger.h"
#include <pthread.h>
#include <cstring>
#include <sstream>

void* loop(void *arg)
{
	char* c = (char*) arg;

	for (long int i = 0; i < 100000; i++)
	{
		CppLogger::logDebug(c);
		CppLogger::logError(c);
	}

	return NULL;
}

int main()
{
	const int size = 100;
	pthread_t threads[size];

	for (int i = 0; i < size; i++)
	{
		std::ostringstream ss;
		ss << i+1;
		size_t len = ss.str().length();
		char* str = new char [ len + 1 ];
		strcpy( str, ss.str().c_str() );
		pthread_create(&threads[i], NULL, loop, str);
	}

	for (int i = 0; i < size; i++)
	{
		pthread_join(threads[i], NULL);
	}

	return 0;
}
