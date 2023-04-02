#include "ScriptEngine.h"
#include "App.h"

ScriptEngine::ScriptEngine() {
    this->L = luaL_newstate();
    luaL_openlibs(this->L);
}

void ScriptEngine::init() {
    registerFunction("renderTemplate", renderTemplate);
    registerFunction("serveStatic", serveStatic);
    registerFunction("fetch", fetch);
    registerFunction("jsonify", jsonify);
}
void ScriptEngine::close() { lua_close(this->L); }

void ScriptEngine::watchChanges() {
    printf("Watching changes\n");
    for (auto kvp : this->_scripts)
    {
        if(kvp.second.watcher != nullptr){
            kvp.second.watcher->join();
        }
    };
}
void ScriptEngine::setupWatchers()
{
    printf("%d files to watch\n", _scripts.size());
    for (auto kvp : this->_scripts)
    {
        printf("added script %s to watch\n",kvp.first.c_str());
        auto t = new boost::thread(
            boost::bind(&ScriptEngine::watchFile, this, kvp.first));
        kvp.second.watcher = t;
    };
}
void ScriptEngine::watchFile(const std::string path)
{
    printf("[debug] Watching %s\n", path.c_str());
    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        if (_scripts.contains(path) && boost::filesystem::exists(path)) {
            auto s = _scripts[path];
            auto last_write_time = boost::filesystem::last_write_time(path);
            if (s.last_write < last_write_time)
            {
                s.last_write = last_write_time;
                _scripts[path] = s;
                // printf("[lua] File changed: %s\n", path.c_str());
                if (luaL_dofile(this->L, path.c_str()) != LUA_OK) {
                    printf("[lua] Error reloading file: %s\n", path.c_str());
                    printf("%s", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
                printf("[lua] '%s' reloaded.\n", path.c_str());
                boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            }
        } else {
            printf("[lua] %s was removed\n", path.c_str());
            std::terminate();
        }
        // sleep(100);
    }
}

bool ScriptEngine::registerFunction(std::string name, lua_CFunction function) {
    lua_pushcfunction(this->L, function);
    lua_setglobal(this->L, name.c_str());
    return true;
}
bool ScriptEngine::loadModule(std::string filename) {
    //   scripts
    if(!boost::filesystem::exists(filename))
        return false;
    ScriptInfo info;
    info.path = filename;
    try{
    info.last_write = boost::filesystem::last_write_time(info.path);
}
    catch(std::exception ex){
        std::cerr << ex.what();
    }
    _scripts[filename] = info;
    return luaL_dofile(this->L, filename.c_str()) == LUA_OK;
}

bool ScriptEngine::loadModuleS(std::string script_content) {
    luaL_dostring(this->L, script_content.c_str());
    return false;
}

int ScriptEngine::getInt(std::string name) {
    lua_getglobal(this->L, name.c_str());
    if (lua_isinteger(this->L, -1)) {

        lua_Number value = lua_tonumber(this->L, -1);
        return (int)value;
    }

    return 0;
}

float ScriptEngine::getFloat(std::string name) {
    lua_getglobal(this->L, name.c_str());
    if (lua_isnumber(this->L, -1)) {
        lua_Number value = lua_tonumber(this->L, -1);
        return (float)value;
    }

    return 0;
}

std::string ScriptEngine::getString(std::string name) {
    lua_getglobal(this->L, name.c_str());
    if (lua_isstring(this->L, -1)) {
        std::string value = std::string(lua_tostring(this->L, -1));
        return value;
    }

    return "";
}

std::map<std::string, std::string> ScriptEngine::getTable(std::string name) {
    std::map<std::string, std::string> result;

    lua_getglobal(this->L, name.c_str());
    lua_pushnil(this->L);
    while (lua_next(this->L, -2) != 0) {
        auto key = lua_tostring(this->L, -2);
        auto value = lua_tostring(this->L, -1);
        // printf("%s: %s\n", key, value);
        result.insert(std::make_pair(key, value));
        lua_pop(this->L, 1);
    }
    lua_pop(this->L, 1);

    return result;
}

lua_State *ScriptEngine::getLuaState() { return this->L; }

// ScriptEngine *ScriptEngine::getInstance()
// {
//     if (instance == NULL){
//         instance = new ScriptEngine();
//     }
//     return instance;
// }
