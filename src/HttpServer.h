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

// typedef struct HttpServer{
//     int port;
//     int socket;
// }HttpServer;

// HttpServer* initHttpServer(HttpServer* server,int port);
class App;
class HttpServer{

    int _port;
    int _socket;
    int _clientSocket;

//    Protocol protocol;

    SSL *ssl;
    SSL_CTX *ctx;
    SSL_METHOD *client_method;
    X509 *server_cert;

    App *_app;
    Router *_router;

    // std::map<int, std::string> statusMessages;
    std::string getStatusMessage(int status_code);

    // Store api endpoints as key value pair of the form (endpoint,lua table)
    // Example: ['/','index']
    //          ['/clients/names','clients.names']
    // std::map<std::string, std::string> _apiRoutes;

public:
    HttpServer(int port,App* app); 
    ~HttpServer();
    void init();
    void exit();

    Router* router();
    // Called in the main loop
    bool tick();
    void sendResponse(const std::string& response_data, const std::string& contentType, int status_code);
};

#endif