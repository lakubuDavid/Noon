#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include "lua.hpp"
#include <map>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include "LuaExt.h"
#include "Middlewares.h"

class App;
typedef struct ScriptInfo {
  boost::filesystem::path path; 
  boost::thread *watcher = nullptr;
  time_t last_write;
} ScriptInfo;

typedef struct ScriptInfo FileInfo;

typedef void (*FileChangeCallback) (FileInfo file);

class ScriptEngine {
    App* _app;
    lua_State *L;
    // static ScriptEngine* instance;
    std::map<std::string, ScriptInfo> _scripts;
    void _watchFile(const std::string& path);

    bool _isOpen = false;
  public:
    ScriptEngine(App* app);

    void init();
    void open();
    void close();
    [[nodiscard]] bool isOpen() const { return _isOpen; }
    void watchChanges();
    void setupWatchers();
    FileInfo watchFile(const std::string& path, FileChangeCallback onChangeCallback);

    bool loadModule(const std::string& filename);
    bool loadModuleS(std::string script_content);

    void reload();

    bool registerFunction(const std::string& name, lua_CFunction function);

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
