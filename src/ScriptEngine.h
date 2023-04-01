#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include "lua.hpp"
#include <map>
#include <string>
#include <vector>


#include "LuaExt.h"

class App;
struct ScriptInfo {
  std::string path;
  // ADD : last modification time
};
class ScriptEngine {
    lua_State *L;
    // static ScriptEngine* instance;
    std::map<std::string, ScriptInfo> scripts;

  public:
    ScriptEngine();

    void init();
    void close();
    void watchChanges();

    bool load(std::string filename);
    bool loadS(std::string script_content);
    bool loadModule(std::string filename);
    bool loadModuleS(std::string script_content);

    bool registerFunction(std::string name, lua_CFunction function);

    int getInt(std::string name);
    float getFloat(std::string name);
    std::string getString(std::string name);

    std::map<std::string, std::string> getTable(std::string name);

    lua_State *getLuaState();
    // static ScriptEngine* getInstance();
};

#endif