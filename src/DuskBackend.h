//
// Created by David Lakubu on 12/04/2023.
//

#ifndef DUSKBACKEND_H
#define DUSKBACKEND_H

#include "Utils.h"
#include <map>
#include <string>
#include <lua.hpp>

using namespace std;
class DuskBackend {
public:
    Result<map<string,string> > run(const std::string& path,lua_State* L);
};


static const char* SERVER_EXPORT_TABLE = "___serverDatas";

int luaExportData(lua_State* L);

#endif //DUSKBACKEND_H
