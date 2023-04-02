#include "HttpServer.h"
#include "App.h"

#include "LuaExt.h"
#include "lua.h"
#include "lua.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fstream>
#include <sstream>

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
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(server_socket, 5);

    this->_socket = server_socket;
}

bool HttpServer::tick() {
    // boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
    char client_msg[4096] = "";

    this->_clientSocket = accept(this->_socket, NULL, NULL);

    read(this->_clientSocket, client_msg, 4095);
    printf("%s\n", client_msg);

    // parsing client socket header to get HTTP method, route
    char *method = "";
    char *urlRoute = "";

    char *client_http_header = strtok(client_msg, "\n");

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

    char *response_data = "<html><body>404</body></html>";
    bool shouldFreeResponseData = false;
    int status_code = 404;
    auto contentType = "text/plain; charset=utf-8";

    auto endpoint = this->router()->getEndpoint(urlRoute);

    // Serve a static file as an asset (only the server is allowed to access it)
    if (std::string(urlRoute).find("/assets") == 0) {
    } else {
        // Check the router for the endpoint
        lua_State *L = this->_app->script()->getLuaState();

        // Call the lua endpoint
        // printf("Checking lua route:%s\n", urlRoute);
        lua_getglobal(L, endpoint.path.c_str());
        if (lua_istable(L, -1)) {
            lua_pushstring(L, method);
            lua_gettable(L, -2);
            if (lua_isfunction(L, -1)) {
                response_data = "";
                lua_pushvalue(L, -2); // Push the table onto the stack
                int status = lua_pcall(L, 1, 2, 0);
                if (status == LUA_OK) {
                    if (lua_isstring(L, -2)) {
                        response_data = (char *)lua_tostring(L, -2);
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
                        response_data = (char *)lua_tostring(L, -1);
                        status_code = 200;
                    }
                    lua_pop(L, 2);

                } else {
                    const char *errorMsg = lua_tostring(
                        L, -1);    // Get the error message from the stack
                    lua_pop(L, 1); // Pop the error message from the stack
                    std::cerr << errorMsg << std::endl;
                }
            } else {
                std::cout << "Error: Can't call method " << method
                          << " on route " << endpoint.path << std::endl;
            }
        } else {
            printf("Checking static file:%s\n", urlRoute);

            std::string path = std::string("./static") + urlRoute;
            printf("Full path: %s\n", path.c_str());

            if (boost::filesystem::exists(path)) {
                // FIXME : This should read and return a static file but I have no clue of how I'm supppsed to do it without segmentation faults 🥲
            } else {
                std::cout << "Error: Can't get route \"" << urlRoute << "\""
                          << std::endl;
            }
        }
    }

    // Return the response data

    std::stringstream http_header;

    if (strlen(response_data) >= 1) {
        if (response_data[0] == '<') {
            contentType = "text/html; charset=utf-8";
        } else if (response_data[0] == '{') {
            contentType = "application/json";
        }
    }
    http_header << "HTTP/1.1 " << status_code << " "
                << getStatusMessage(status_code) << std::endl;
    http_header << "Content-Type: " << contentType << std::endl;
    http_header << "Server: Noon 1.0 alpha" << std::endl;
    http_header << "X-Powered-By:Lua 5.4" << std::endl;
    http_header << "\r\n\r\n";

    std::string chttp_header = http_header.str();

    const size_t response_size =
        strlen(chttp_header.c_str()) + strlen(response_data);

    char *response = (char *)malloc(response_size);

    strcat(response, chttp_header.c_str());
    strcat(response, response_data);
    strcat(response, "\r\n\r\n");

    // FIXME : The header disapears from the response when it's an html response

    printf("%s",response);

    send(this->_clientSocket, response, response_size, 0);
    close(this->_clientSocket);
    if (shouldFreeResponseData) {
        free(response_data);
        shouldFreeResponseData = false;
    }
    free(response);
    return true;
}

Router *HttpServer::router() { return this->_router; }