//
// Created by David Lakubu on 10/04/2023.
//

#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include "Connection.h"

class SocketConnection : public Connection {

    int _socket = -1;
public:
    SocketConnection(const std::string& host,bool is_server=false);
    SocketConnection(int socket);
    virtual int open();
    virtual void close();

    virtual int listen();
    virtual int accept();

    virtual int write(char* buffer, int buffer_len);
    virtual int read(char* buffer, int buffer_len);
};


#endif //SOCKETCONNECTION_H
