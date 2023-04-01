#include "ScriptEngine.h"
#include "App.h"

ScriptEngine::ScriptEngine()
{
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
  for (auto path : this->scripts) {
    
  }
}

bool ScriptEngine::registerFunction(std::string name, lua_CFunction function) {
  lua_pushcfunction(this->L, function);
  lua_setglobal(this->L, name.c_str());
}
bool ScriptEngine::loadModule(std::string filename) {
//   scripts
    return luaL_dofile(this->L, filename.c_str()) == LUA_OK;
}

bool ScriptEngine::loadModuleS(std::string script_content)
{
    luaL_dostring(this->L, script_content.c_str());
    return false;
}

bool ScriptEngine::load(std::string filename)
{
    luaL_loadfile(this->L, filename.c_str());
    return false;
}

bool ScriptEngine::loadS(std::string script_content)
{
    luaL_loadstring(this->L, script_content.c_str());
    return false;
}

int ScriptEngine::getInt(std::string name)
{
    lua_getglobal(this->L, name.c_str());
    if (lua_isinteger(this->L, -1))
    {

        lua_Number value = lua_tonumber(this->L, -1);
        return (int)value;
    }

    return 0;
}

float ScriptEngine::getFloat(std::string name)
{
    lua_getglobal(this->L, name.c_str());
    if (lua_isnumber(this->L, -1))
    {
        lua_Number value = lua_tonumber(this->L, -1);
        return (float)value;
    }

    return 0;
}

std::string ScriptEngine::getString(std::string name)
{
    lua_getglobal(this->L, name.c_str());
    if (lua_isstring(this->L, -1))
    {
        std::string value = std::string(lua_tostring(this->L, -1));
        return value;
    }
    
    return "";
}

std::map<std::string, std::string> ScriptEngine::getTable(std::string name)
{
    std::map<std::string, std::string> result;

    lua_getglobal(this->L, name.c_str());
    lua_pushnil(this->L);
    while(lua_next(this->L,-2)!=0){
        auto key = lua_tostring(this->L, -2);
        auto value = lua_tostring(this->L, -1);
        //printf("%s: %s\n", key, value);
        result.insert(std::make_pair(key, value));
        lua_pop(this->L, 1);
    }
    lua_pop(this->L, 1);

    return result;
}

lua_State *ScriptEngine::getLuaState()
{
    return this->L;
}

// ScriptEngine *ScriptEngine::getInstance()
// {
//     if (instance == NULL){
//         instance = new ScriptEngine();
//     }
//     return instance;
// }
