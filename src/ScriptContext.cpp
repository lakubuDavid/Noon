#include "ScriptContext.h"
#include "App.h"
#include "RuntimeConfig.h"
#include "KVStore.h"

ScriptContext::ScriptContext(App *app) : _app(app) {
	this->L = luaL_newstate();
	luaL_openlibs(this->L);
}

void ScriptContext::init() {
	open();

	// Common functions
	{
		registerFunction("renderTemplate", renderTemplate);
		registerFunction("serveStatic", serveStatic);
		registerFunction("fetch", fetch);
		registerFunction("jsonify", jsonify);
		registerFunction("parseJson", json_to_lua);
	}

	//Middleware functions
	useExtension(new MiddlewareExtension);
	useExtension(new KVStoreExtension);

	//We set up the global context table that will be used by middleware and other parts of the server
	{
		lua_newtable(L);

		//Middleware stuff
		{
			lua_pushstring(L, "middleware");
			lua_newtable(L);

			//Add the middleware stack
			{
				lua_pushstring(L, "stack");
				lua_newtable(L);
				lua_settable(L, -3);
			}

			lua_settable(L, -3);
		}
		lua_setglobal(L, "___context");
	}

	//Set package.path to allow imports
	{
		std::string path;
		path = boost::filesystem::absolute(RuntimeConfig::instance()->get("NOON_APP_DIR")).string() + "/libs/?.lua";
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "path"); // get field "path" from table at top of stack (-1)
		std::string cur_path = lua_tostring(L, -1); // grab path string from top of stack
		cur_path.append(";"); // do your path magic here
		cur_path.append(path);
		lua_pop(L, 1); // get rid of the string on the stack we just pushed on line 5
		lua_pushstring(L, cur_path.c_str()); // push the new one
		lua_setfield(L, -2, "path"); // set the field "path" in table at -2 with value at top of stack
		lua_pop(L, 1); // get rid of package table from top of stack
	}

}

void ScriptContext::open() {
	this->L = luaL_newstate();
	luaL_openlibs(this->L);
	_isOpen = true;
}

void ScriptContext::close() {
	lua_close(this->L);
	_isOpen = false;
}

void ScriptContext::watchChanges() {
	printf("Watching changes\n");
	for (auto kvp: this->_scripts) {
		if (kvp.second.watcher != nullptr) {
			kvp.second.watcher->join();
		}
	};
}

void ScriptContext::setupWatchers() {
	printf("%lu files to watch\n", _scripts.size());
	for (auto kvp: this->_scripts) {
		printf("added script %s to watch\n", kvp.first.c_str());
		auto t = new boost::thread(
				[this, capture0 = kvp.first] { _watchFile(capture0); });
		kvp.second.watcher = t;
	};
}

void ScriptContext::_watchFile(const std::string &path) {
	printf("[debug] Watching %s\n", path.c_str());
	while (true) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		if (_scripts.contains(path) && boost::filesystem::exists(path)) {
			auto s = _scripts[path];
			auto last_write_time = boost::filesystem::last_write_time(path);
			if (s.last_write < last_write_time) {
				s.last_write = last_write_time;
				_scripts[path] = s;
				reload();
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

bool ScriptContext::registerFunction(const std::string &name, lua_CFunction function) {
	lua_pushcfunction(this->L, function);
	lua_setglobal(this->L, name.c_str());
	return true;
}

bool ScriptContext::loadModule(const std::string &filename) {
	//   scripts
	if (!boost::filesystem::exists(filename))
		return false;
	if (!_scripts.contains(filename)) {
		ScriptInfo info;
		info.path = filename;
		try {
			info.last_write = boost::filesystem::last_write_time(info.path);
		}
		catch (const std::exception &ex) {
			std::cerr << ex.what();
		}
		_scripts[filename] = info;
	}
	return luaL_dofile(this->L, filename.c_str()) == LUA_OK;
}

bool ScriptContext::loadModuleS(std::string script_content) {
	luaL_dostring(this->L, script_content.c_str());
	return false;
}

int ScriptContext::getInt(std::string name) {
	lua_getglobal(this->L, name.c_str());
	if (lua_isinteger(this->L, -1)) {

		lua_Number value = lua_tonumber(this->L, -1);
		return (int) value;
	}

	return 0;
}

float ScriptContext::getFloat(std::string name) {
	lua_getglobal(this->L, name.c_str());
	if (lua_isnumber(this->L, -1)) {
		lua_Number value = lua_tonumber(this->L, -1);
		return (float) value;
	}

	return 0;
}

std::string ScriptContext::getString(std::string name) {
	lua_getglobal(this->L, name.c_str());
	if (lua_isstring(this->L, -1)) {
		std::string value = std::string(lua_tostring(this->L, -1));
		return value;
	}

	return "";
}

std::map<std::string, std::string> ScriptContext::getTable(std::string name) {
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

lua_State *ScriptContext::getLuaState() { return this->L; }

void ScriptContext::reload() {
	lua_close(L);
	this->L = luaL_newstate();
	luaL_openlibs(this->L);
	init();
	for (auto [path, info]: this->scripts()) {
		luaL_dofile(L, path.c_str());
	}
}

FileInfo ScriptContext::watchFile(const std::string &path, FileChangeCallback onChangeCallback) {
	FileInfo info;
	info.path = path;
	info.last_write = boost::filesystem::last_write_time(path);
	auto t = new boost::thread(
			[this, file = &info, callback = onChangeCallback] {
				printf("Watching %s for changes...\n", file->path.c_str());
				while (true) {
					sleep(100);
//                    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
					if (boost::filesystem::exists(file->path)) {
						auto last_write_time = boost::filesystem::last_write_time(file->path);
						if (file->last_write < last_write_time) {
							file->last_write = last_write_time;

							printf("%s was changed\n", file->path.c_str());
							callback(*file);
							boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
						}
					} else {
						printf("%s was removed\n", file->path.c_str());
						std::terminate();
					}
				}
			});
//    t->join();
	info.watcher = t;
	return info;
}

void ScriptContext::useExtension(ILuaExtension *extension) {
	extension->registerExtension(this->L);
}

// ScriptEngine *ScriptEngine::getInstance()
// {
//     if (instance == NULL){
//         instance = new ScriptEngine();
//     }
//     return instance;
// }
