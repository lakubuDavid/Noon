#ifndef LUA_EXT_H
#define LUA_EXT_H

#include <string>
#include <cstring>
#include <lua.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <map>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#include <cmath>

#include <nlohmann/json.hpp>

#include "Utils.h"
#include "Log.h"

#include "SSLConnection.h"
#include "SocketConnection.h"
#include "interfaces/ILuaExtension.h"

struct FetchResult {
	char *data;
	char *error;
};

char *nativeServeStatic(const std::string &path);

int serveStatic(lua_State *L);

std::string nativeRenderTemplate(const std::string &path, std::map<std::string, std::string> params, lua_State *L);

int renderTemplate(lua_State *L);

void jsonify_table(lua_State *L, nlohmann::json &obj);

int jsonify(lua_State *L);

int json_to_lua(lua_State *L);

void json_array_to_lua(lua_State *L, nlohmann::json &json_array);

void json_object_to_lua(lua_State *L, nlohmann::json &json_obj);
//int parseJson(lua_State *L);

char *nativeFetch(std::string url, std::string method, std::string data = "", std::string headers = "");

int fetch(lua_State *L);

int getConfig(lua_State *L);

class CommonLuaExtension : public ILuaExtension {
public:
	virtual void registerExtension(lua_State *L) override;
};

#endif /* LUA_EXT_H */
