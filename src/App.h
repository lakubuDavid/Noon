#ifndef APP_H
#define APP_H

#include "lua.hpp"
#include <string>
#include <iostream>
#include <fstream>

#include "ScriptContext.h"
#include "Server.h"

class App {
	lua_State *lua_State;
	ScriptContext *scriptEngine;
	Server *server;

	int _port = 8080;
	bool _debugMode = false;
	static App *instance;
	bool running;
public:
	App();

	virtual ~App();

	int run();

	void close();

	ScriptContext *script();

	void setDebugMode(bool devMode);

	bool loadConfig();

	static void onConfigChanged(FileInfo info);
};

#endif