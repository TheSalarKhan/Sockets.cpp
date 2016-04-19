/*
 * WorkerThread.cpp
 *
 *  Created on: Dec 31, 2015
 *      Author: msalar
 */

#include "WorkerThread.h"

typedef unsigned char byte;

WorkerThread::WorkerThread(Socket* connection) :
		_connection(connection) {
	// TODO Auto-generated constructor stub

}

WorkerThread::~WorkerThread() {
	// TODO Auto-generated destructor stub
}

std::string WorkerThread::readString() {

	std::vector<byte> dataBuffer;
	// wait until you receive a command.
	if(_connection->readData(dataBuffer)) {
		// null terminate the command string.
		dataBuffer[dataBuffer.size()] = '\0';

		return std::string((char *) (&dataBuffer[0]));
	} else {
		perror("WorkerThread::readString() ");
		pthread_exit(NULL);
	}

	return "";
}

void WorkerThread::writeString(std::string message) {
	std::vector<byte> dataBuffer(message.begin(), message.end());

	dataBuffer.push_back('\0');

	_connection->transmitData(dataBuffer);
}
void WorkerThread::run() {

	do {

		parse(readString());

	} while (_connection->isConnected());

	_connection->end();
	delete _connection;
}

std::vector<std::string> WorkerThread::split(std::string str, char delimeter) {
	std::vector<std::string> elems;
	std::stringstream ss(str);
	std::string item;

	while (std::getline(ss, item, delimeter)) {
		elems.push_back(item);
	}

	return elems;
}

std::string WorkerThread::cd(std::string path)
{
	if(chdir(path.c_str()) == -1)
	{
			return "error: no such directory";
	}
	else
	{
		if(!strcmp(path.c_str(),"."))
			return "";
		else if(!strcmp(path.c_str(),".."))
		{
			std::string current(getenv("PWD"));

			if(current == "/") return "";

			std::string temp = current;

			int i;
			for(i=current.length()-1;i>=0;i--)
			{
				if(current[i] == '/')
				{
					temp[i] = '\0';
					break;
				}
			}
			if(temp.length() == 0)
				setenv("PWD","/",1);
			else
				setenv("PWD",temp.c_str(),1);
		}
		else if(!strcmp(path.c_str(),"/"))
		{
			setenv("PWD","/",1);
		}
		else
		{
			std::string temp = "";

			std::string ptr(getenv("PWD"));

			if(ptr == "/") ptr = "\0";

			temp += ptr + "/" + path;

			setenv("PWD",temp.c_str(),1);
		}
	}

	return "";
}


std::string WorkerThread::ls(std::string arg)
{
	DIR *d_ptr;

	std::string toReturn="";

	struct dirent *d;
	d_ptr = opendir(arg.c_str());

	if(d_ptr == NULL)
	{
		puts("\nError in opening directory");
		return "cannot open directory";
	}

	while((d = readdir(d_ptr))!= NULL)
	{
		if(d->d_type == DT_DIR)
			toReturn+= std::string(d->d_name)+"    DIR"+"\n";
		else
			toReturn+= std::string(d->d_name)+"    FILE"+"\n";

	}

	return toReturn;
}

std::string WorkerThread::rm(std::string arg)
{
	if(fileExists(arg)) {
		remove(arg.c_str());
		return "File remove successfully";
	}

	return "No such file";
}

std::string WorkerThread::cp(std::string file1,std::string file2)
{
	FILE *fp1, *fp2;
	char a;

	fp1 = fopen(file1.c_str(), "rb");
	if (fp1 == NULL) {
	  //puts("cannot read file");
	  return "Cannot read file";
	}

	fp2 = fopen(file2.c_str(), "wb");
	if (fp2 == NULL) {
	  //puts("Cannot open file for writing");
	  fclose(fp1);
	  return "Cannot open file for writing";
	}

	while((a = fgetc(fp1)) != EOF)
	{
	  fputc(a, fp2);
	}

	fclose(fp1);
	fclose(fp2);

	return "Copied Successfully";
}

bool WorkerThread::fileExists(std::string filename)
{
   FILE *fp = fopen (filename.c_str(), "r");
   if (fp!=NULL) fclose (fp);
   return (fp!=NULL);
}

void WorkerThread::parse(std::string command) {

	std::cout << "Client wrote: " << command << std::endl;
	std::vector<std::string> tokens = split(command, ' ');
	if (tokens[0] == "ls") {

		if(tokens.size() > 1)
			writeString(ls(tokens[1]));
		else
			writeString(ls("."));

	} else if (tokens[0] == "cd") {
		if(tokens.size() > 1)
			writeString(cd(tokens[1]));
		else
			writeString("usage: cd <path>");

	} else if (tokens[0] == "cp") {

		if(tokens.size() == 3) {
			writeString(cp(tokens[1],tokens[2]));
		} else
			writeString("usage: cp <source file> <destination file>");

	} else if (tokens[0] == "cpr") {



	} else if (tokens[0] == "rm") {

		if(tokens.size() == 2) {
			writeString(rm(tokens[1]));
		} else {
			writeString("usage: rm <file path>");
		}


	} else if (tokens[0] == "pwd") {

		writeString(getenv("PWD"));


	} else if (tokens[0] == "exit") {
		// Exit gracefully
		_connection->end();

	} else {

		writeString("Invalid command");
	}
}

