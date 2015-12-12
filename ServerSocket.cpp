/*
 * ServerSocket.cpp
 *
 *  Created on: Nov 22, 2015
 *      Author: msalar
 */

#include "ServerSocket.h"

ServerSocket::ServerSocket(std::string port) {

	// This is the struct that we partially fill, before a call to getaddrinfo.
	struct addrinfo hints;

	// Clean the struct, for it is created on the stack.
	memset(&hints, 0, sizeof hints);

	// Keeping in mind that we need to bind this socket to an ip on this machine.
	// I don't actually care what type of address you use for this job.
	hints.ai_family = AF_UNSPEC;

	// I want a TCP socket.
	hints.ai_socktype = SOCK_STREAM;

	// Yes! thats right use my IP
	hints.ai_flags = AI_PASSIVE;

	// To hold the value returned by getaddrinfo()
	int rv;

	// This pointer will hold the starting address of the linked list
	// that is returend by getaddrinfo()
	struct addrinfo* servinfo;

	rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo);


	// If an error occurs.
	// print the error and return.
	if (rv != 0) {
		fprintf(stderr, "ServerSocket::ServerSocket() %s\n", gai_strerror(rv));
		_sd = -1;
		return;
	}

	// This is a temporary pointer to iterate over the iist of addresses
	// returned by getaddrinfo().
	struct addrinfo* p;

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {

		// Try to get a socket for this address
		_sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		// If an error occured.
		// Print the error, and move on to the next address.
		if ( _sd == -1) {
			perror("ServerSocket::ServerSocket() ");
			continue;
		}

		// We want to tell the OS to reuse the port, and to not wait, in case the port has just been
		// freed by another process and is waiting to be renewed.
		// For accomplishing this we will use the setsockopt() function to change the value of the SO_REUSEADDR option.
		// The following variable holds the truth value that we want to set for the option.
		int yes = 1;
		// Here we reuse 'rv' to hold our return value.
		rv = setsockopt(_sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		// If an error has occured.
		// Print the error, and move on to the next address.
		if ( rv	== -1) {
			perror("ServerSocket::ServerSocket() on using setsockopt(), ");
			continue;
		}

		// Else, bind the resources for this address. If bind returns an error
		// close the socket descriptor and move on to the nezt address in the list.
		if (bind(_sd, p->ai_addr, p->ai_addrlen) == -1) {
			close(_sd);
			perror("ServerSocket::ServerSocket() on using bind(), ");
			continue;
		}

		// If we have reached the end, that means we now have a binded address.
		// So free the linked list returned by getaddrinfo() and break from the loop.
		freeaddrinfo(servinfo); // all done with this structure
		break;
	}

	// If out temporary pointer is still null, that means we were unable to bind.
	// print the error and return from here.
	if (p == NULL) {
		fprintf(stderr, "SererSocket::ServerSocket() failed to bind\n");
		_sd = -1;
		return;
	}

	// Now register this socket, as a server socket with a queue for pending connections,
	// having length 'BACKLOG'. If however, this returns an error.
	// Print the error, close the socket descriptor, and return.
	if (listen(_sd, BACKLOG) == -1) {
		perror("ServerSocket::ServerSocket() on using listen(), ");
		close(_sd);
		_sd = -1;
		return;
	}

}

ServerSocket::~ServerSocket() {
	// TODO Auto-generated destructor stub
}

Socket ServerSocket::acceptIncoming() {

	if(_sd == -1) {
		perror("ServerSocker::acceptIncoming() socket not initialized properly, socket descriptor missing.");
		Socket a;
		return a;
	}
	// Integer type to hold the size of the incoming socket.
	socklen_t sin_size;

	// Connector's address information.
	struct sockaddr_storage their_addr;

	sin_size = sizeof their_addr;

	// Now accept an incoming connection.
	int incomingSD = accept(_sd, (struct sockaddr *) &their_addr, &sin_size);

	// If there was an error in accepting the connection.
	// Print the error, and return null.
	if (incomingSD == -1) {
		perror("ServerSocket::acceptIncoming() error in accepting connection. ");
		Socket a;
		return a;
	}

	// Else, get the connection information.
	// Get Ip address
	char s[INET6_ADDRSTRLEN];
	struct sockaddr* sa = (struct sockaddr*) &their_addr;

	void *in_addr = (sa->sa_family == AF_INET)?
			(void *)(&(((struct sockaddr_in*) sa)->sin_addr)) :
			(void *)(&(((struct sockaddr_in6*) sa)->sin6_addr));
	inet_ntop(their_addr.ss_family, in_addr, s, sizeof s);
	std::string theirIp(s);

	// Get port number.
	unsigned int portNumber=0;

	if (sa->sa_family == AF_INET) {
		portNumber = ((struct sockaddr_in*) sa)->sin_port;
	} else {
		portNumber = ((struct sockaddr_in6*) sa)->sin6_port;
	}
	std::stringstream out;
	out << portNumber;
	std::string port = out.str();

	Socket toReturn(incomingSD,theirIp,port);
	return toReturn;
}


void ServerSocket::end() {
	close(_sd);
	_sd = -1;
}
