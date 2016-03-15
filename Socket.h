/*
 * Socket.h
 *
 *  Created on: Nov 22, 2015
 *      Author: msalar
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <fcntl.h>

#define MAXDATASIZE 2048

typedef unsigned char byte;

class Socket {
public:
	Socket();

	// Initialize the socket with a socket descriptor
	Socket(int sd,std::string ip,std::string port);

	virtual ~Socket();

	/**
	 * Connect to a server socket listening on 'ip:port'. This is a blocking function.
	 *
	 * @params hostname: The host name or the ip address of the remote server
	 * 		   port: Port number on which the remote server is listening.
	 * @returns 'true' if the connection was successfully established, else it returns 'false'
	 */
	bool connectTo(std::string hostname,std::string port);

	/**
	 * Read data from the stream.
	 *
	 * @params data: this is the reference of the vector in which the received data will be placed.
	 * @returns 'true' if the data was read successfully, else returns 'false'.
	 */
	bool readData(std::vector<byte> &data);

	/**
	 * Write data to the stream.
	 *
	 * @params data: this is the reference of the vector containing the data to be transmitted. All of the data in it will be sent.
	 * @returns 'true' on success, else returns 'false'.
	 */
	bool transmitData(std::vector<byte> &data);


	bool isConnected();

	int getSd();

	std::string getPort();

	std::string getIP();

	/**
	 * End the socket by closing the socket descriptor
	 */
	void end();
private:

	// Socket descriptor for the current connection, if any.
	int _sd;

	// Ip for the connected remote end.
	std::string _ip;

	// Port number for the connected remote end.
	std::string _port;
};

#endif /* SOCKET_H_ */
