#include "LuaExt.h"
#include "json.h"
#include "lua.h"
#include <cstdio>

using namespace std;

std::string nativeRenderTemplate(std::string path) {
    auto file = std::ifstream(path);
    if (!file.is_open()) {
        std::cerr << "Error opening " << path << std::endl;
        // throw;
        return "";
    }
    auto html = std::stringstream();
    std::string line;

    while (std::getline(file, line)) {
        html << line << std::endl;
    }
    return html.str();
}

int renderTemplate(lua_State *L) {
    if (lua_isstring(L, -1)) {
        auto path = lua_tostring(L, -1);
        auto html = nativeRenderTemplate(path);
        lua_pushstring(L, html.c_str());
    } else {
        lua_Debug debug_info;
        lua_getstack(L, 1, &debug_info);
        lua_getinfo(L, "Sl", &debug_info);
        std::cerr << debug_info.short_src << ":" << debug_info.currentline
                  << "Expected a string as an input" << std::endl;
        return 0;
    }
    return 1;
}

char *nativeServeStatic(std::string path) {
    std::ifstream file;
    file.open(path);
    if (!file.is_open()) {
        std::cerr << "Error opening " << path << std::endl;
        // throw;
        return "";
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char *buffer = (char *)malloc(size);
    // if (!file.read(buffer, size)) {
    //   std::cerr << "Error reading data "<<path << std::endl;
    //     throw;
    // }
    std::string line;
    while (std::getline(file, line)) {
        strcat(buffer, line.c_str());
    }
    file.close();
    return buffer;
}

int serveStatic(lua_State *L) {
    if (lua_isstring(L, -1)) {
        auto path = lua_tostring(L, -1);
        auto ret = nativeServeStatic(path);
        lua_pushstring(L, ret);
        return 1;
    }
    return 0;
}

void jsonify_table(lua_State *L, json::jobject &obj) {
    int type;
    const char *key;
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        type = lua_type(L, -1);
        key = lua_tostring(L, -2);
        switch (type) {
        case LUA_TSTRING:
            obj[key] = (lua_tostring(L, -1));
            break;
        case LUA_TNUMBER:
            obj[key] = (lua_tonumber(L, -1));
            break;
        case LUA_TBOOLEAN:
            obj[key] = (lua_toboolean(L, -1));
            break;
        case LUA_TTABLE:
            json::jobject sub_obj;
            jsonify_table(L, sub_obj);
            obj[key] = sub_obj;
            break;
            // default:
            // break;
        }
        lua_pop(L, 1);
    }
}

int jsonify(lua_State *L) {
    luaL_checktype(L, -1, LUA_TTABLE);

    json::jobject obj;
    jsonify_table(L, obj);

    std::string json_str = obj.as_string();
    lua_pushstring(L, json_str.c_str());

    return 1;
}

int json_to_lua(lua_State *L) {
    if (!lua_isstring(L, -1)) {
        return 0;
    }
    lua_newtable(L);
    string json_str = lua_tostring(L, -1);
    // Parse the JSON string
    json::jobject json = json::jobject::parse(json_str);

    for (int i = 0; i < json.size(); i++) {
    }

    return 1;
}
void json_array_to_lua(lua_State *L, json::jobject &json_array) {}
void json_object_to_lua(lua_State *L, json::jobject &json_obj) {}

char *nativeFetch(std::string url, std::string method, std::string data) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << "[lua] : Could not create socket" << endl;
        return "";
    }

    size_t pos = url.find("://");
    string protocol = url.substr(0, pos);
    url.erase(0, pos + 3);

    pos = url.find("/");
    string host = url.substr(0, pos);
    string path = url.substr(pos);
    printf("[fetch] : Fetching %s from %s | %s\n", path.c_str(), host.c_str(),
           method.c_str());

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(host.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(80);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        cerr << "[fetch] : Connection failed" << endl;
        return NULL;
    }

    string request = method + " " + path + " HTTP/1.1\r\nHost: " + host +
                     "\r\nConnection: close\r\n";
    if (method == "POST") {
        request += "Content-Type: "
                   "application/x-www-form-urlencoded\r\nContent-Length: " +
                   to_string(data.length()) + "\r\n";
        request += "\r\n" + data;
    } else if (method == "PUT") {
        request += "Content-Type: "
                   "application/x-www-form-urlencoded\r\nContent-Length: " +
                   to_string(data.length()) + "\r\n";
        request += "\r\n" + data;
    } else if (method == "DELETE") {
        request += "\r\n";
    } else {
        request += "\r\n";
    }
    try {
        if (send(sock, request.c_str(), request.length(), 0) < 0) {
            cerr << "[fetch] : Send failed" << endl;
            return NULL;
        }

        char buffer[1024] = {0};
        string response = "";
        while (read(sock, buffer, sizeof(buffer)) > 0) {
            response += buffer;
            memset(buffer, 0, sizeof(buffer));
        }

        close(sock);

        char *result = new char[response.length() + 1];
        strcpy(result, response.c_str());

        return result;
    } catch (const std::exception ex) {
        char *error;
        snprintf(error, sizeof(error), "[fetch] : Error: %s", ex.what());
        return error;
    }
}

int fetch(lua_State *L) {
    if (lua_isstring(L, -3) && lua_isstring(L, -2)) {
        string url = lua_tostring(L, -3);
        string method = lua_tostring(L, -2);
        string data = "";
        printf("[lua] : fetching %s | %s\n", url.c_str(), method.c_str());
        if ((method == "POST" || method == "PUT") && !lua_isstring(L, -1)) {
            // If the method is POST or PUT, the 3rd argument should be
            // provided as a string
            // TODO : send back an error message to lua
            lua_pushnil(L);
            return 1;
        } else {
            data = lua_tostring(L, -1);
        }

        char *fdata = nativeFetch(url, method, data);
        long timer = time(NULL);
        if (fdata == NULL) {
          cerr << "Nothing fetched from " << url << endl;
            lua_pushnil(L);
        } else {
          timer = time(NULL) - timer;
          cout << timer << "ms elapsed"<<endl;
            lua_pushstring(L, fdata);
        }
        return 1;
    }
    // First 2 args are mandatory
    // TODO : send back an error message to lua
    printf("[lua] : Invalid arguments for fetch");
    lua_pushnil(L);
    return 1;
}