#ifndef LUA_EXT_H
#define LUA_EXT_H

#include <string>
#include "lua.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>

#include <cmath>

#include "json.h"
#include <nlohmann/json.hpp>

struct FetchResult {
  char *data;
  char *error;
};

char *nativeServeStatic(const std::string& path);
int serveStatic(lua_State *L);

std::string nativeRenderTemplate(const std::string& path,std::map<std::string,std::string> params,lua_State *L);
int renderTemplate(lua_State *L);

void jsonify_table(lua_State* L, json::jobject& obj);
int jsonify(lua_State *L);

int json_to_lua(lua_State* L);
void json_array_to_lua(lua_State* L, nlohmann::json& json_array);
void json_object_to_lua(lua_State *L, nlohmann::json& json_obj);
//int parseJson(lua_State *L);

char *nativeFetch(std::string url,std::string method,std::string data="");
int fetch(lua_State* L);

#endif /* LUA_EXT_H */
