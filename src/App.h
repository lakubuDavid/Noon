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
    bool _debugMode = false;
    static App* instance;
    bool running;
public:
    App(int port=8080);
    virtual ~App();

    int run();
    ScriptEngine* script();

    void setDebugMode(bool devMode);

    bool loadConfig();

    static void onConfigChanged(FileInfo info);
};

#endif