/*
 * includes.h
 *
 *  Created on: Oct 4, 2015
 *      Author: msalar
 */

//#include "ServerSocket.h"
#include <pthread.h>

#include "ServerSocket.h"
#include <iostream>

//
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <errno.h>
//#include <string.h>
//#include <netdb.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <sys/socket.h>
//#include <sys/wait.h>
//#include <arpa/inet.h>


//#include <signal.h>

#define PORT "3490" // the port client will be connecting to


#define BACKLOG 10     // how many pending connections queue will hold


void* echoServer(void* something);
void* client(void* something);

bool RUNNING = true;

void* echoServer(void* something) {

	ServerSocket server(PORT);
	Socket incoming = server.acceptIncoming();
	server.end(); // stop listening for more connections.

	// A buffer for transmission and reception.
	std::vector<byte> data;

	// Transmit a welcome message.
	std::string toTransmit = "Welcome to server on Salar's machine: ";
	data.reserve(toTransmit.length());
	for(register unsigned int i=0;i<toTransmit.length();i++) {
		data.push_back(toTransmit[i]);
	}
	incoming.transmitData(data);



	while(1) {


		// read the data
		if(incoming.readData(data)) {
			data[data.size()] = '\0';
			std::cout << "Received(size): " << (char *)(&data[0]) << std::endl;


			// transmit the same data.
			incoming.transmitData(data);
		} else {
			break;
		}
	}

	incoming.end();
	server.end();

	pthread_exit(NULL);
}

void* client(void* something) {

	Socket connection;

	std::cout << "Connecting to server..." << std::endl;


	connection.connectTo("192.168.0.100","11111");

	std::cout << "Connected" << std::endl;



	//connection.readData(dataReceived);

//	for(int i=0;i<dataReceived.size();i++) {
//		std::cout << (char)dataReceived[i] << std::endl;
//	}

	std::string input;
	std::vector<byte> dataToSend;

	while(1) {

		std::cout << "Please enter data: ";
		getline(std::cin,input);

		dataToSend.clear();
		dataToSend.reserve(input.length());
		for(int i=0;i<input.length();i++)
			dataToSend.push_back(input[i]);

		connection.transmitData(dataToSend);
	}

	pthread_exit(NULL);
}

int main() {
	//pthread_t serverThread;

	client((void *)1);


	//printf("Creating server thread");
	//int status = pthread_create(&serverThread, NULL, echoServer, (void *) NULL);

	//printf("Bye Bye");
	//pthread_exit(0);
}

