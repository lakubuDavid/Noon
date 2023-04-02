#ifndef APP_H
#define APP_H

#include "lua.hpp"
#include <string>
#include <iostream>
#include <fstream>

#include "ScriptEngine.h"
#include "HttpServer.h"

class App {
    lua_State *lua_State;
    ScriptEngine *scriptEngine;
    HttpServer *httpServer;

    int _port = 8080;
    bool _devMode = false;
public:
    App(int port=8080);
    virtual ~App();
    void renderTemplate(std::string template_name, void *context);

    int run();
    ScriptEngine* script();

    void setDevMode(bool devMode);
};

#endif