#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include "lua.hpp"
#include <map>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include "LuaExt.h"

class App;
typedef struct ScriptInfo {
  boost::filesystem::path path; 
  boost::thread *watcher = nullptr;
  time_t last_write;
} ScriptInfo;
class ScriptEngine {
    lua_State *L;
    // static ScriptEngine* instance;
    std::map<std::string, ScriptInfo> _scripts;
    void watchFile(const std::string path);

  public:
    ScriptEngine();

    void init();
    void close();
    void watchChanges();
    void setupWatchers();

    bool loadModule(std::string filename);
    bool loadModuleS(std::string script_content);

    bool registerFunction(std::string name, lua_CFunction function);

    int getInt(std::string name);
    float getFloat(std::string name);
    std::string getString(std::string name);

    std::map<std::string, std::string> getTable(std::string name);

    lua_State *getLuaState();
    inline std::map<std::string, ScriptInfo> scripts(){
      return _scripts;
    }

    // static ScriptEngine* getInstance();
};

#endif /* SCRIPT_ENGINE_H */
