//
// Created by David Lakubu on 12/04/2023.
//

#include "DuskBackend.h"


Result<map<string,string> > DuskBackend::run(const std::string& path,lua_State* L){
    Result<map<string,string> > result;

    lua_pushcfunction(L, luaExportData);
    lua_setglobal(L,"export");

    luaL_dofile(L,path.c_str());
    return result;
}

int luaExportData(lua_State *L) {
    if(!lua_istable(L,-1)){
        return 0;
    }
    lua_gettable(L,-1);
    lua_pushvalue(L,-1);
    lua_setglobal(L,SERVER_EXPORT_TABLE);

    return 0;
}