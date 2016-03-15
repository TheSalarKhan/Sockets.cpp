/*
 * includes.h
 *
 *  Created on: Oct 4, 2015
 *      Author: msalar
 */

//#include "ServerSocket.h"
#include <pthread.h>

#include "ServerSocket.h"
#include "WorkerThread.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <fstream>
#include <numeric>





//#include <signal.h>

#define PORT "12345" // the port client will be connecting to


#define BACKLOG 10     // how many pending connections queue will hold


void* echoServer(void* something);
void* client(void* something);

bool RUNNING = true;

// This is the worker thread for the echo server.
void* workerThread(void* socket) {

	// Create a worker entity.
	WorkerThread worker((Socket*)socket);

	// Call run on the entity
	worker.run();

	pthread_exit(NULL);
}


void* Server(void* something) {

	ServerSocket server(PORT);

	while(1) {
		// Listen for a client's connection.
		// As soon as we receive a connection. We'll put it on a seperate worker thread
		// and we will let this thread go back to listening for more connections.
		Socket* incoming = server.acceptIncoming();

		std::cout << "Client connected: " << std::endl;
		std::cout << "Their IP is: " << incoming->getIP() << std::endl;
		std::cout << "Their port is: " << incoming->getPort() << std::endl;

		pthread_t new_thread;
		new_thread = pthread_create(&new_thread, NULL, workerThread, (void *) incoming);
		incoming = nullptr;

	};




	server.end(); // stop listening for more connections.


	pthread_exit(NULL);
}

bool fileExists(std::string filename)
{
   FILE *fp = fopen (filename.c_str(), "r");
   if (fp!=NULL) fclose (fp);
   return (fp!=NULL);
}

std::vector<std::string> split(std::string str, char delimeter) {
	std::vector<std::string> elems;
	std::stringstream ss(str);
	std::string item;

	while (std::getline(ss, item, delimeter)) {
		elems.push_back(item);
	}

	return elems;
}


int sizeOfFile(std::string file)
{
  FILE * pFile;
  long size;

  pFile = fopen (file.c_str(),"rb");
  if (pFile==NULL) return -1; // unable to open file
  else
  {
    fseek (pFile, 0, SEEK_END);   // non-portable
    size=ftell (pFile);
    fclose (pFile);
    return size;
  }
  return 0;
}


std::vector<byte> readFile(const char * filename)
{
    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<byte> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
               std::istream_iterator<byte>(file),
               std::istream_iterator<byte>());

    return vec;
}



void* client(void* something) {

	Socket connection;

	std::cout << "Connecting to server..." << std::endl;


	if(!connection.connectTo("172.15.6.122","12345")) {
		pthread_exit(NULL);
	}

	std::cout << "Connected" << std::endl;




	std::string input;


	do {

		std::cout << "Please enter data: ";
		getline(std::cin,input);

		std::vector<std::string> tokens = split(input,' ');

		if(tokens.size() == 0) // If enter was pressed without any string
			continue;

		if(tokens[0] != "cpr") {
			std::vector<byte> dataToSend(input.begin(),input.end());
			dataToSend[dataToSend.size()] = '\0';

			connection.transmitData(dataToSend);

			connection.readData(dataToSend);
			std::cout << ((char*)&dataToSend[0]) << std::endl;
		} else {




			if(tokens.size() == 2 && fileExists(tokens[1])) {
				// send file transfer flag to the server
				std::string token = "cpr";
				std::vector<byte> dataToSend(token.begin(),token.end());
				dataToSend[dataToSend.size()] = '\0';
				connection.transmitData(dataToSend);

				// wait for a green signal
				connection.readData(dataToSend);

				if(std::string(((char*)&dataToSend[0])) == "OK") // Upon receiving green flag, the server is waiting for the file size.
				{

					dataToSend = readFile(tokens[1].c_str());

					//std::string str = "";

					int sizeofFile = sizeOfFile(tokens[1]);
					std::stringstream ss;
					ss << dataToSend.size();
					std::string str = ss.str();
					str += "\0";

					std::vector<byte> sizeToTransfer(str.begin(),str.end());

					connection.transmitData(sizeToTransfer);








				}
			} else {

				std::cout << "usage: cpr <localfile>";

			}





		}


	} while(connection.isConnected());

	pthread_exit(NULL);
}

int main() {

	Server((void *)1);


	return 0;

}

