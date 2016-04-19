/*
 * WorkerThread.h
 *
 *  Created on: Dec 31, 2015
 *      Author: msalar
 */

#ifndef WORKERTHREAD_H_
#define WORKERTHREAD_H_

#include "Socket.h"
#include <iostream>
#include <sstream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <errno.h>
#include <dirent.h>

class WorkerThread {
public:
	WorkerThread(Socket* connection);
	virtual ~WorkerThread();

	void run();



private:

	// Takes a string read from the socket and makes sense of it
	void parse(std::string message);


	std::string readString();

	void writeString(std::string message);

	// Helper method to split strings, with respect to a delimeter
	std::vector<std::string> split(std::string str,char delimeter);

	std::string ls(std::string);

	std::string cd(std::string path);

	std::string rm(std::string arg);

	std::string cp(std::string file1,std::string file2);

	bool fileExists(std::string filename);


	// The connection assigned to this thread.
	Socket* _connection;

	std::string _pwd;
};

#endif /* WORKERTHREAD_H_ */
