#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "Router.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <map>
#include <string>
#include <nlohmann/json.hpp>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


class App;

class Server {

    int _port;
    int _socket;

    int _clientSocket;
    SSL *_clientSSL;
//    Protocol protocol;

    SSL *ssl;
    SSL_CTX *ctx;
    SSL_METHOD *client_method;
    X509 *server_cert;

    App *_app;
    Router *_router;

    // std::map<int, std::string> statusMessages;
    std::string getStatusMessage(int status_code);


public:
    Server(int port, App *app);

    ~Server();

    void init();

    void exit();

    std::string getIpAddress();

    Router *router();

    // Called in the main loop
    bool httpListen();

    bool httpsListen();

    bool listen(bool secure = false);

    void sendResponse(std::string response_data, std::string contentType, int status_code, bool useSSL = false);

    bool handleRequest(char *request, bool useSSL = false);

};

#endif