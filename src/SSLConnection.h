//
// Created by David Lakubu on 10/04/2023.
//

#ifndef SSLCONNECTION_H
#define SSLCONNECTION_H

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include "Connection.h"

class SSLConnection : public Connection {
    int _clientSocket;
    int _socket = -1;
    SSL *ssl;
    SSL_CTX *ctx;
    SSL_METHOD *client_method;
    X509 *server_cert;
    BIO* bio;
public:
    SSLConnection(const std::string& host,bool is_server=false);
    SSLConnection(int socket);
    virtual int open();
    virtual void close();

    virtual int listen();
    virtual int accept();

    virtual int write(char* buffer, int buffer_len);
    virtual int read(char* buffer, int buffer_len);

    void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile);
    void ShowCerts(SSL* ssl);

};


#endif //SSLCONNECTION_H
