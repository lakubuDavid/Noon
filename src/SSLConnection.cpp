//
// Created by David Lakubu on 10/04/2023.
//

#include "SSLConnection.h"

SSLConnection::SSLConnection(const std::string &host, bool is_server) :
Connection(host, is_server) {

}
SSLConnection::SSLConnection(int socket) {
    _socket = socket;
}

int SSLConnection::open() {
    std::cout << "Opening SSL connection" << std::endl;
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    if(_socket == -1) {
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

        }
        else {
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
                std::cerr << "Failed to open : Can't find "<<_info.host <<":"<<_info.port << std::endl;
                return -1;
            }
        }
    }
    if(_isServer){
        ctx = SSL_CTX_new(TLS_server_method());
    }else{
        ctx = SSL_CTX_new(TLS_client_method());
    }
    if(ctx == NULL) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    if (SSL_CTX_use_certificate_file(ctx, "noon_cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "noon_cert.pem", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
       return -1;
    }
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, _socket);

    int connect_result =  SSL_connect(ssl);
    {
        X509 *cert;
        char *line;
        cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
        if ( cert != NULL )
        {
            printf("Server certificates:\n");
            line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
            printf("Subject: %s\n", line);
            free(line);       /* free the malloc'ed string */
            line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
            printf("Issuer: %s\n", line);
            free(line);       /* free the malloc'ed string */
            X509_free(cert);     /* free the malloc'ed certificate copy */
        }
        else
            printf("Info: No client certificates configured.\n");
    }
    return connect_result;
}

void SSLConnection::close() {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    ::close(_socket);
}

int SSLConnection::write(char *buffer, int buffer_len) {
    return SSL_write(ssl,buffer,buffer_len);
}

int SSLConnection::read(char *buffer, int buffer_len) {
    return SSL_read(ssl,buffer,buffer_len);
}

int SSLConnection::listen() {
    return ::listen(_socket,1);
}

int SSLConnection::accept() {
    return SSL_accept(ssl);
}

