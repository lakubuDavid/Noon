//
// Created by David Lakubu on 10/04/2023.
//

#include "SocketConnection.h"

int SocketConnection::open() {
	std::cout << "Opening socket connection" << std::endl;
	if (_socket != -1)
		return _socket;
	if (_isServer) {
		_socket = socket(AF_INET, SOCK_STREAM, 0);

		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(this->_info.porti);
		server_addr.sin_addr.s_addr = INADDR_ANY;
		if (bind(_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
			::close(_socket);
			return -1;
		}

		::listen(_socket, 10);
	} else {
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;


		struct addrinfo *result;
		int status = getaddrinfo(_info.host.c_str(), _info.port.c_str(), &hints, &result);
		if (status != 0) {
			std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
			return -1;
		}

		struct addrinfo *p;
		for (p = result; p != NULL; p = p->ai_next) {
			_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (_socket == -1) {
				continue;
			}

			if (::connect(_socket, p->ai_addr, p->ai_addrlen) == -1) {
				::close(_socket);
				continue;
			}
			break;
		}

		freeaddrinfo(result);


		if (p == NULL) {
			std::cerr << "Failed to open" << std::endl;
			return -1;
		}
	}
	return _socket;
}

void SocketConnection::close() {
	::close(_socket);
}

SocketConnection::SocketConnection(const std::string &host, bool is_server) :
		Connection(host, is_server) {}

int SocketConnection::write(char *buffer, int buffer_len) {
	return ::write(_socket, buffer, buffer_len);
}

int SocketConnection::read(char *buffer, int buffer_len) {
	return ::read(_socket, buffer, buffer_len);
}

int SocketConnection::listen() {
	return ::listen(_socket, 1);
}

int SocketConnection::accept() {
	return ::accept(_socket, NULL, NULL);
}

SocketConnection::SocketConnection(int socket) {
	_socket = socket;
}
