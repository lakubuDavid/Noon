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

#include <math.h>

#include "json.h"

struct FetchResult {
  char *data;
  char *error;
};

char *nativeServeStatic(std::string path);
int serveStatic(lua_State *L);

std::string nativeRenderTemplate(std::string path);
int renderTemplate(lua_State *L);

void jsonify_table(lua_State* L, json::jobject& obj);
int jsonify(lua_State *L);

void json_to_lua(lua_State* L, std::string json_str);
void json_array_to_lua(lua_State* L, json::jobject& json_array);
void json_object_to_lua(lua_State *L, json::jobject &json_obj);

char *nativeFetch(std::string url,std::string method,std::string data="");
int fetch(lua_State* L);

#endif /* LUA_EXT_H */
