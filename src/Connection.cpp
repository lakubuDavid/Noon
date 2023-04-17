//
// Created by David Lakubu on 10/04/2023.
//

#include "Connection.h"

using namespace std;

ConnectionInfo Connection::getInfo(const std::string &host, bool secureByDefault) {
    ConnectionInfo info;
    auto url = host;
    size_t pos = url.find("://");

    string protocol;
    if(pos!=string::npos) {
        protocol = url.substr(0, pos);
        url.erase(0, pos + 3);
    }else{
        if(secureByDefault)
            protocol = "https";
        else
            protocol = "http";
    }
    if( protocol == "http")
        info.protocol = PROTOCOL_HTTP;
    else if( protocol == "https")
        info.protocol = PROTOCOL_HTTPS;

    pos = url.find('/');

    auto colon_pos = url.find(':');
    if (colon_pos != string::npos) {
        info.port = url.substr(colon_pos + 1, pos - colon_pos - 1);
        info.host = url.substr(0, colon_pos);
    }
    else {
        info.host = url.substr(0, pos);
        if (protocol == "http")
            info.port = "80";
        else if (protocol == "https")
            info.port = "443";
    }

    info.porti = parse_int(info.port);

    return info;
}

Connection::~Connection() {
    close();
}

Connection::Connection(const std::string &host, bool is_server) {
    _isServer = is_server;
    if(!is_server){
        _info = Connection::getInfo(host);
    }
}

Connection::Connection() {
    _isServer = false;

}
