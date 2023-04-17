#include "HttpServer.h"
#include "App.h"

#include "LuaExt.h"
#include "lua.hpp"
#include "StaticFile.h"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

std::string HttpServer::getStatusMessage(int status_Code) {
    static std::map<int, std::string> statusMessages;
    statusMessages[200] = "OK";
    statusMessages[201] = "Created";
    statusMessages[202] = "Accepted";
    statusMessages[204] = "No Content";
    statusMessages[301] = "Moved Permanently";
    statusMessages[302] = "Found";
    statusMessages[304] = "Not Modified";
    statusMessages[400] = "Bad Request";
    statusMessages[401] = "Unauthorized";
    statusMessages[403] = "Forbidden";
    statusMessages[404] = "Not Found";
    statusMessages[500] = "Internal Server Error";
    statusMessages[501] = "Not Implemented";
    statusMessages[502] = "Bad Gateway";
    statusMessages[503] = "Service Unavailable";
    auto it = statusMessages.find(status_Code);
    if (it != statusMessages.end()) {
        return it->second;
    } else {
        return "";
    }
}

HttpServer::HttpServer(int port, App *app) {
    this->_port = port;
    this->_app = app;
    this->_router = new Router(app);
}

void HttpServer::init() {
    int err;
    //Initialize SSL
    {
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        // Create SSL context
        ctx = SSL_CTX_new(TLS_server_method());
    }
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // SSL handshake
    {
        ssl = SSL_new(ctx); /* create SSL stack endpoint */
        SSL_set_fd(ssl, server_socket); /* attach SSL stack to socket */
        err = SSL_connect(ssl); /* initiate SSL handshake */
    }
    SSL_CTX_set_cipher_list(ctx, "ALL:eNULL");
    printf("(5) SSL connected with cipher: %s\n\n", SSL_get_cipher(ssl));
    // SSL  certificate
    {
        server_cert = SSL_get_peer_certificate(ssl);

        printf("(6) server's certificate was received:\n\n");
        if (server_cert != nullptr)
        {
            printf("Server certificates:\n");
            char *line = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
            printf("Subject: %s\n", line);
            delete line;
            line = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
            printf("Issuer: %s\n", line);
            delete line;
            X509_free(server_cert);
        }
        else
        {
            printf("Info: No client certificates configured.\n");
        }
//        auto str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
//        printf(" subject: %s\n", str);
//
//        str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
//        printf(" issuer: %s\n\n", str);

        /* certificate verification would happen here */

        X509_free(server_cert);
    }

    bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));

    listen(server_socket, 5);

    this->_socket = server_socket;
}

bool HttpServer::tick() {
    // boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
    char client_request[4096] = "";

    this->_clientSocket = accept(this->_socket, NULL, NULL);
    if (_clientSocket == -1) {
        return true;
    }
    read(this->_clientSocket, client_request, 4095);
    printf("%s\n", client_request);

    // parsing client socket header to get HTTP method, route
    char *method = "";
    char *urlRoute = "";

    char *client_http_header = strtok(client_request, "\n");

    // printf("\n\n%s\n\n", client_http_header);

    char *header_token = strtok(client_http_header, " ");

    int header_parse_counter = 0;

    while (header_token != NULL) {
        switch (header_parse_counter) {
            case 0:
                method = header_token;
            case 1:
                urlRoute = header_token;
        }
        header_token = strtok(NULL, " ");
        header_parse_counter++;
    }

    char reqv[4096] = "";
    for (int i = 0; i < 4095; i++) {
        if (client_request[i] == '\0') {
            if (client_request[i + 1] == '\0') {
                break;
            }
            reqv[i] = ' ';
        } else {
            reqv[i] = client_request[i];
        }
    }

    char *response_data = "<html><body>404</body></html>";
    int status_code = 404;
    auto contentType = "text/html; charset=utf-8";

    if (strlen(urlRoute) == 0)
        urlRoute = "/";
    auto endpoint = this->router()->getEndpoint(urlRoute);

    bool shouldFreeResponseData = false;
    {
        _app->script()->init();
        lua_State *L = _app->script()->getLuaState();

        if (boost::filesystem::exists("routes/" + endpoint.endpoint)) {
            if (_app->script()->loadModule("routes/" + endpoint.endpoint)) {

                /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                 * Create and push the request object that will contain information about requests
                 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                 * The request object will look like this :
                 * {
                 *     body : table | nil // for POST and PUT requests
                 *     query : table
                 *     path : string
                 *     url : string
                 * }
                 * */
                lua_newtable(L); // Request table
                //Query parameters
                {
                    //We pass each query parameter to lua
                    lua_pushstring(L, "query");
                    lua_newtable(L); // Query table
                    for (auto const &pair: endpoint.parameters) {
                        lua_pushstring(L, pair.first.c_str());
                        lua_pushstring(L, pair.second.c_str());
                        lua_settable(L, -3);
                    }
                    lua_settable(L, -3);
                }
                //If POST or PUT, body parameter
                if (strcmp(method, "POST") == 0 || strcmp(method, "PUT") == 0) {
                    auto req = std::string(reqv);
                    int pos = req.find('{');
                    if (pos != std::string::npos) {
                        auto post_data = req.substr(pos);
                        auto json_data = nlohmann::json::parse(post_data);
                        lua_pushstring(L, "body");
                        lua_newtable(L); // Body table
                        json_object_to_lua(L, json_data);
                        lua_settable(L, -3);
                    }
                }
                //Route parameters
                {
                    lua_pushstring(L, "path");
                    lua_pushstring(L, endpoint.path.c_str());
                    lua_settable(L, -3);

                    lua_pushstring(L, "url");
                    lua_pushstring(L, endpoint.url.c_str());
                    lua_settable(L, -3);
                }

                lua_setglobal(L, "request"); // We have a global request object


                /*
                 * Now we have to handle middlewares
                 *
                 * We have a global ___context object that holds everything about the execution context
                 * We will store each middleware in the global ___context
                 * The context will look like this :
                 * ___context = {
                 *    middleware = {
                 *        stack // Will contain middlewares as a stack
                 *    }
                 * }
                 */
                // 1. Get all the middlewares from the global ___context table
                lua_getglobal(L, "___context");
                lua_getfield(L, -1, "middleware");
                lua_getfield(L, -1, "stack");
                std::vector<std::string> middlewares;
                lua_pushnil(L);
                while (lua_next(L, -2) != 0) {
                    if (!lua_isstring(L, -1)) {
                        break;
                    }
                    std::string value = lua_tostring(L, -1);
                    // printf("%s: %s\n", key, value);
                    middlewares.push_back(value);
                    lua_pop(L, 1);
                }
                lua_pop(L, 1);

                // 2. We load and execute each one of them
                for (const auto &middleware: middlewares) {
                    luaL_dofile(L, middleware.c_str());
                    lua_getglobal(L, "RouteHandler");
                    if (lua_isfunction(L, -1)) {
                        // We pull the current middleware parameters from ___middleware_params and use the middleware
                        lua_getglobal(L, "___middleware_params");
                        lua_pcall(L, 1, 1, 0);
                        auto result = (MiddlewareResult) lua_tointeger(L, -2);

                        if (result == MIDDLEWARE_RESULT_ABORT) {
                            // We need to abort and return the error code
                            sendResponse(response_data, contentType, status_code);
                            return true;
                        } else if (result == MIDDLEWARE_RESULT_REDIRECT) {
                            // TODO
                        }
                    }
                }

                // Since all middlewares have been called we can now call the actual route handler

                lua_getglobal(L, method);
                if (lua_isfunction(L, -1)) {
                    response_data = "";
                    int status = lua_pcall(L, 0, 2, 0);
                    if (status == LUA_OK) {
                        if (lua_isstring(L, -2)) {
                            response_data = (char *) lua_tostring(L, -2);
                            if (lua_isnoneornil(L, -1)) {
                                status_code = 200;
                            }
                        } else if (lua_isnoneornil(L, -2)) {
                            response_data = "";
                            if (lua_isnoneornil(L, -1)) {
                                status_code = 200;
                            }
                        }
                        if (lua_isinteger(L, -1))
                            status_code = lua_tointeger(L, -1);
                        else if (lua_isstring(L, -1)) {
                            response_data = (char *) lua_tostring(L, -1);
                            status_code = 200;
                        }
                        lua_pop(L, -2);

                        if (response_data != NULL && strlen(response_data) >= 1) {
                            if (response_data[0] == '<') {
                                contentType = "text/html; charset=utf-8";
                            } else if (response_data[0] == '{') {
                                contentType = "application/json";
                            }
                        }
                    } else {
                        const char *errorMsg = lua_tostring(
                                L, -1);    // Get the error message from the stack
                        lua_pop(L, 1); // Pop the error message from the stack
                        std::cerr << "[lua] : " << errorMsg << std::endl;
                    }
                } else {
                    std::cout << "Error: Can't call method " << method << " on route " << endpoint.path << std::endl;
                }
            } else {
                std::cerr << "[lua] : Can't find route " << urlRoute << " at /routes/" << endpoint.endpoint
                          << std::endl;
            }
        } else {
            if (boost::filesystem::exists("static/" + std::string(urlRoute))) {
                // FIXME : This only works with text based files ,I have no clue how I'm supposed to do it without segmentation faults 🥲
                try{
                    File file;
                    openFile("static" + std::string(urlRoute),file);
                    auto r = std::string(urlRoute);
                    switch (file.type) {
                        case FILE_TYPE_IMAGE_PNG:
                            contentType = "image/png";
                            break;
                        case FILE_TYPE_IMAGE_JPEG:
                            contentType = "image/jpeg";
                            break;
                        case FILE_TYPE_IMAGE_GIF:
                            contentType = "image/gif";
                            break;
                        case FILE_TYPE_TEXT:
                            if (r.ends_with(".js"))
                                contentType = "application/javascript";
                            else if (r.ends_with(".html") || r.ends_with(".htm"))
                                contentType = "text/html";
                            else if (r.ends_with(".json"))
                                contentType = "application/json";
                            else if (r.ends_with(".xml"))
                                contentType = "application/xml";
                            else if (r.ends_with(".css"))
                                contentType = "text/css";

                            break;
                        case FILE_TYPE_UNKNOWN:
                        case FILE_TYPE_OTHER:
                            contentType = "application/octet-stream";
                            break;
                    }
                    if(file.type != FILE_TYPE_TEXT){
                        response_data = (char *) malloc(file.size);
                        std::memcpy(response_data, file.data, file.size);
                        shouldFreeResponseData = true;
                    }else{
                        response_data = file.data;
                    }

                    status_code=200;
                }
                catch (const std::exception& ex){
                    std::cerr << "Could not load file: "<<urlRoute <<" : " << ex.what() << std::endl;
                }
            }
        }
    }

    // Return the response data

    sendResponse(response_data, contentType, status_code);

    if (shouldFreeResponseData) {
        free(response_data);
        shouldFreeResponseData = false;
    }
    return true;
}

Router *HttpServer::router() { return this->_router; }

void HttpServer::sendResponse(const std::string &response_data, const std::string &contentType, int status_code) {
    std::stringstream http_header;

    http_header << "HTTP/1.1 " << status_code << " "
                << getStatusMessage(status_code) << std::endl;
    http_header << "Content-Type: " << contentType << std::endl;
    http_header << "Server: Noon 1.0 alpha" << std::endl;
    http_header << "X-Powered-By:Lua 5.4" << std::endl;
    http_header << "\r\n\r\n";

    std::stringstream resp;
    resp << http_header.str() << response_data << "\r\n\r\n";

    std::string cresp = resp.str();

    const size_t response_size = strlen(cresp.c_str());

    char *response = (char *) malloc(response_size);

    strcat(response, cresp.c_str());

    size_t totalBytesSent = 0;
    size_t bytesToSend = response_size;
    while (totalBytesSent < bytesToSend) {
        auto bytesSent = send(this->_clientSocket, response + totalBytesSent, bytesToSend - totalBytesSent, 0);
        totalBytesSent += bytesSent;
//        int err = SSL_write(ssl,response,response_size);
    }
    _app->script()->close();
    close(this->_clientSocket);
    free(response);
}

void HttpServer::exit() {
    SSL_shutdown(ssl);
    close(this->_socket);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}

HttpServer::~HttpServer() {
    this->exit();
}
