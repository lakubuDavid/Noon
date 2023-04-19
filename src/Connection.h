//
// Created by David Lakubu on 10/04/2023.
//

#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <iostream>
#include "Utils.h"

enum Protocol{
    PROTOCOL_HTTP,
    PROTOCOL_HTTPS
};

typedef struct ConnectionInfo{
    std::string host;
    std::string port;
    std::string path;
    int porti;
    Protocol protocol;
} ConnectionInfo;

class Connection {
protected:
    ConnectionInfo _info;
    bool _isServer;
public:
    Connection(const std::string &host, bool is_server);
    Connection();
    virtual  ~Connection();
    virtual int open() = 0;
    virtual void close() = 0;

    virtual int write(char* buffer, int buffer_len) = 0;
    virtual int read(char* buffer, int buffer_len) = 0;
    virtual int listen()=0;

    virtual int accept() = 0;
    static ConnectionInfo getInfo(const std::string &host, bool secureByDefault = true);
};



#endif //CONNECTION_H
