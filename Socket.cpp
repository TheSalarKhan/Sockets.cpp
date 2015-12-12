/*
 * Socket.cpp
 *
 *  Created on: Nov 22, 2015
 *      Author: msalar
 */

#include "Socket.h"

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

Socket::Socket(): _sd(-1) {

}

Socket::Socket(int sd,std::string ip, std::string port): _sd(sd), _ip(ip), _port(port)  {

}

Socket::~Socket() {
	// TODO Auto-generated destructor stub
}

bool Socket::isConnected() {
	if(_sd != -1)
		return true;

	return false;
}

bool Socket::connectTo(std::string hostname,std::string port) {

	// If the socket is already connected, end the connection
	if(_sd != -1) {
		end();
	}

	// This is the struct we partially fill, before a call to getaddrinfo().
	struct addrinfo hints;

	// We clean it using memset, because it was created on the stack.
	memset(&hints, 0, sizeof hints);

	// This pointer will be referring to a linked list of addrinfo structs,
	// when the getaddrinfo() returns.
	struct addrinfo* servinfo;

	// For storing the return value of getaddrinfo().
	int rv;

	hints.ai_family = AF_UNSPEC; // addrinfo family unspecified, get me any IPv6 or IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP type socket

	rv = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &servinfo);

	// If any error occurs, print the error and return false.
	if (rv != 0) {
		perror(gai_strerror(rv));
		return false;
	}

	// A temporary pointer to iterate over the list of addrinfo returned by getaddrinfo().
	struct addrinfo* p;

	// Else, loop through all the structs returned and connect to the first we can.
	for (p = servinfo; p != NULL; p = p->ai_next) {

		// request a socket for this address
		_sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		// If we cannot get the socket, print the error and move on to the next address.
		if ( _sd == -1) {
			perror("Socket::connectTo() ");
			continue;
		}

		// Else, connect to the socket. If we're not able to connect:
		// close the socket descriptor.
		// print the error and move on to the next address.
		if (connect(_sd, p->ai_addr, p->ai_addrlen) == -1) {
			close(_sd);
			perror("Socket::connectTo() ");
			continue;
		}

		// If we get to the bottom, that means we have successfully connected to the client
		break;
	}

	// If we were not able to connect to any of the addresses returned by getaddrinfo().
	// Print an error and return 'false'.
	if (p == NULL) {
		perror("Socket::connectTo() failed to connect");
		return false;
	}

	// Now that we have connected successfully, we'll get the IP of the connection.
	// And we will save it in the _ip attribute of this object.
	char s[INET6_ADDRSTRLEN];
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s, sizeof s);
	_ip = s;

	// We'll also save the port too.
	_port = port;

	// When we are done, we'll finally free the linked list returned by getaddrinfo().
	freeaddrinfo(servinfo);

	return true;
}

bool Socket::readData(std::vector<byte> &data)  {
	// Clear the vector.
	data.clear();

	char* buffer = (char *)malloc(sizeof(char) * MAXDATASIZE);

	// Attempt to read data from the stream.
	int lenRead = recv(_sd,(void* )buffer,MAXDATASIZE,0);

	// If there was an error on recv.
	// Print the error, and return false.
	if(lenRead == -1) {
		perror("Socket::readData() ");
		return false;
	}

	// Copy the received data, to the vector.
	data.insert( data.begin() , buffer , buffer + lenRead ) ;


	// Free the buffer.
	free(buffer);

	// Read successful.
	return true;
}

bool Socket::transmitData(std::vector<byte> &data) {

	// Get the number of bytes of data that needs to be sent.s
	unsigned int numBytesToSend = data.size();

	// This variable will keep an account of the number of bytes that have been sent.
	unsigned int numBytesSent = 0;


	while(numBytesToSend) {
		// Send returns with the number of Bytes transmitted.
		// The address given to the send function is computed as follows:
		// 				Starting address of the data + number of bytes already sent.
		int BytesSent = send( _sd, ((byte *)&data[0]) , numBytesToSend , 0 );
		if(BytesSent == -1) {
			perror("Socket::transmitData() ");
			return false;
		}

		// Increment the number of bytes sent.
		numBytesSent += BytesSent;

		// Decrement the number of bytes that are left to be sent.
		numBytesToSend -= BytesSent;
	}

	return true;
}


void Socket::end()  {
	close(_sd);
	_sd = -1;
}
