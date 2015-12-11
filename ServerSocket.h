/*
 * ServerSocket.h
 *
 *  Created on: Nov 22, 2015
 *      Author: msalar
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include "Socket.h"
#include <sstream>


// Length of the queue for pending connections.
#define BACKLOG 10

class ServerSocket {
public:
	ServerSocket(std::string port);

	virtual ~ServerSocket();
	/**
	 * Accept a pending connection from the queue.
	 */
	Socket acceptIncoming();

	void end();
private:
	int _sd;
	std::string _port;
	std::string _ip;
};

#endif /* SERVERSOCKET_H_ */
