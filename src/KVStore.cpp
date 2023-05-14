//
// Created by David Lakubu on 04/05/2023.
//

#include "KVStore.h"
#include "CommonLuaExtensions.h"

using namespace std;

KVStore::KVStore(const std::string &name, bool persist) {
	this->_json = nlohmann::json();
	this->_name = name;
	this->_persist = persist;
	auto p = KVStoreRootPath + noon_conf->get("NOON_APP_NAME", "no_name") + "/";
	if (persist && !boost::filesystem::exists(p)) {
		boost::filesystem::create_directory(p);
	}
	this->_path = p + name + ".json";
}

KVStore::~KVStore() {
	printf("KVStore freed");
	flush();
}


KVStore *KVStore::getStore(std::string name) {
	auto p = KVStoreRootPath + noon_conf->get("NOON_APP_NAME", "no_name") + "/";
	auto path = p + name + ".json";
	if (boost::filesystem::exists(path)) {
		auto json_text = File::readAllText(path);
		KVStore *store = new KVStore(name);
		store->deserialize(nlohmann::json::parse(json_text));
		return store;
	} else {
		return new KVStore(name, true);
	}
}

void KVStore::deleteStore() {
	if (boost::filesystem::exists(_path))
		boost::filesystem::remove(_path);
}

bool KVStore::exists(std::string name) {
	auto path = KVStoreRootPath + noon_conf->get("NOON_APP_NAME", "no_name") + "/" + name + ".json";
	return boost::filesystem::exists(path);
}

KVStoreIndex::KVStoreIndex(std::string value, KVStoreIndexType type) {
	_innerValue = value;
	this->_type = type;
}


KVStoreIndex::KVStoreIndex() {

}

nlohmann::json KVStoreIndex::serialize() {
	auto json = nlohmann::json();
	json["type"] = type();
	json["value"] = _innerValue;
	return json;
}

void KVStoreIndex::deserialize(nlohmann::json serialized) {
	if (serialized.contains("type") && serialized.contains("value")) {
		_innerValue = serialized["value"].get<std::string>();
		_type = serialized["type"].get<KVStoreIndexType>();
	}
}


nlohmann::json KVStore::serialize() {
	nlohmann::json json;
	nlohmann::json content;
	for (auto k: _indexes) {
//		nlohmann::json index;
//		index["key"] = k.first;
//		index["value"] = k.second.serialize();
//		content.push_back(index);
		content[k.first] = k.second.serialize();
	}
	json["name"] = _name;
	json["content"] = content;
//    auto path = KVStoreRootPath + "/" + _name + ".json";
	File::writeAllText(_path, json.dump(4));
	return json;
}

void KVStore::deserialize(nlohmann::json serialized) {
	if (serialized.contains("name") && serialized.contains("content")) {
		_name = serialized["name"].get<string>();
		auto content = serialized["content"];
		for (auto item: content.items()) {
			KVStoreIndex index;
			index.deserialize(item.value());
			_indexes[item.key()] = index;
		}
	}
}

void KVStore::flush() {
	if (_persist) {
		serialize();
	}
}

KVStoreIndex KVStore::get(std::string key) {
	if (_indexes.contains(key))
		return _indexes[key];
	return KVStoreIndex("", KVS_INDEX_TYPE_NULL);
}

void KVStore::set(std::string key, KVStoreIndex value) {
	_indexes[key] = value;
}

std::map<std::string, KVStoreIndex> KVStore::all() {
	return _indexes;
}

void KVStoreExtension::registerExtension(lua_State *L) {
//	lua_State *L = s_context->getLuaState();
	lua_newtable(L);
	int kvstoreTableIndex = lua_gettop(L);
	lua_pushvalue(L, kvstoreTableIndex);
	lua_setglobal(L, "KV");


	lua_pushcfunction(L, [](lua_State *L) -> int {
		std::string name;
		bool persist = false;
		if (lua_isstring(L, -1)) {
			name = lua_tostring(L, -1);
		} else if (lua_isboolean(L, -1) && lua_isstring(L, -2)) {
			name = lua_tostring(L, -2);
			persist = lua_toboolean(L, -1);
		}

		void *storePointer = lua_newuserdata(L, sizeof(KVStore));
		new(storePointer) KVStore(name, persist);
		luaL_getmetatable(L, "KVStore");
		lua_setmetatable(L, -2);
		return 1;
	});
	lua_setfield(L, -2, "createStore");

	lua_pushcfunction(L, [](lua_State *L) -> int {
		std::string name;
		if (lua_isstring(L, -1)) {
			name = lua_tostring(L, -1);
			auto store = KVStore::getStore(name);
			lua_pushlightuserdata(L, store);
			luaL_getmetatable(L, "KVStore");
			lua_setmetatable(L, -2);
		}
		return 1;
	});
	lua_setfield(L, -2, "getStore");

	lua_pushcfunction(L, [](lua_State *L) -> int {
		KVStore *self = (KVStore *) lua_touserdata(L, -1);
		auto serialized = self->serialize().dump();
		lua_pushstring(L, serialized.c_str());
		return 1;
	});
	lua_setfield(L, -2, "serialize");

	lua_pushcfunction(L, [](lua_State *L) -> int {
		KVStore *self = (KVStore *) lua_touserdata(L, -1);
		self->flush();
		return 0;
	});
	lua_setfield(L, -2, "save");

	lua_pushcfunction(L, [](lua_State *L) -> int {
		KVStore *self;
		string key;
		self = (KVStore *) lua_touserdata(L, -2);
		key = lua_tostring(L, -1);

		auto value = self->get(key);
		switch (value.type()) {
			case KVS_INDEX_TYPE_TEXT:
				lua_pushstring(L, value.text().c_str());
				break;
			case KVS_INDEX_TYPE_NUMBER:
				lua_pushnumber(L, value.number());
				break;
			case KVS_INDEX_TYPE_BOOLEAN:
				lua_pushboolean(L, value.boolean());
				break;
			case KVS_INDEX_TYPE_JSON: {
				auto json = value.json();
				json_object_to_lua(L, json);
				break;
			}
			case KVS_INDEX_TYPE_NULL:
				lua_pushnil(L);
				break;
		}
		return 1;
	});
	lua_setfield(L, -2, "get");

	lua_pushcfunction (L, [](lua_State *L) -> int {
		KVStore *self;
		string key;
		self = (KVStore *) lua_touserdata(L, -3);
		key = lua_tostring(L, -2);
		if (lua_isstring(L, -1)) {
			string val = lua_tostring(L, -1);
			KVStoreIndex index(val, KVS_INDEX_TYPE_TEXT);
			self->set(key, index);
		} else if (lua_isboolean(L, -1)) {
			bool val = lua_toboolean(L, -1);
			KVStoreIndex index(std::to_string(val), KVS_INDEX_TYPE_BOOLEAN);
			self->set(key, index);
		} else if (lua_isnumber(L, -1)) {
			float val = lua_tonumber(L, -1);
			KVStoreIndex index(std::to_string(val), KVS_INDEX_TYPE_NUMBER);
			self->set(key, index);
		} else if (lua_isinteger(L, -1)) {
			int val = lua_tointeger(L, -1);
			KVStoreIndex index(std::to_string(val), KVS_INDEX_TYPE_NUMBER);
			self->set(key, index);
		} else if (lua_isnil(L, -1)) {
			KVStoreIndex index("", KVS_INDEX_TYPE_NULL);
			self->set(key, index);
		} else if (lua_istable(L, -1)) {
			nlohmann::json json;
			jsonify_table(L, json);
			KVStoreIndex index(json.dump(), KVS_INDEX_TYPE_JSON);
			self->set(key, index);
		};
		return 0;
	});
	lua_setfield(L, -2, "set");

	lua_pushcfunction(L, [](lua_State *L) -> int {
		KVStore *self;
		self = (KVStore *) lua_touserdata(L, -1);
		auto items = self->all();

		lua_newtable(L);
		int i = 0;
		for (auto item: items) {
//			printf("%s : %s", item.first.c_str(), item.second.serialize().dump().c_str());
			lua_pushstring(L, item.first.c_str());
			if (item.second.type() == KVS_INDEX_TYPE_TEXT) {
				lua_pushstring(L, item.second.text().c_str());
			} else if (item.second.type() == KVS_INDEX_TYPE_NUMBER) {
				lua_pushnumber(L, item.second.number());
			} else if (item.second.type() == KVS_INDEX_TYPE_BOOLEAN) {
				lua_pushboolean(L, item.second.boolean());
			} else if (item.second.type() == KVS_INDEX_TYPE_JSON) {
				auto json = item.second.json();
				lua_newtable(L);
				json_object_to_lua(L, json);
			} else if (item.second.type() == KVS_INDEX_TYPE_NULL) {
				lua_pushnil(L);
			}
			lua_settable(L, -3);
			i++;
		}

		return 1;
	});
	lua_setfield(L, -2, "all");

	luaL_newmetatable(L, "KVStore");

	lua_pushcfunction(L, [](lua_State *L) -> int {
		auto kvstore = (KVStore *) lua_touserdata(L, -1);
		delete kvstore;
		printf("KVStore __gc");
		return 0;
	});
	lua_setfield(L, -2, "__gc");

	lua_pushvalue(L, kvstoreTableIndex);
	lua_setfield(L, -2, "__index");

	(new KVStoreIndexExtension())->registerExtension(L);
}

void KVStoreIndexExtension::registerExtension(lua_State *L) {
//	lua_State *L = s_context->getLuaState();

	luaL_newmetatable(L, "KVStoreIndex");

	lua_pushcfunction(L, [](lua_State *L) -> int {
		auto index = (KVStoreIndex *) lua_touserdata(L, -1);
		printf("KVStoreIndex __index");
		return 0;
	});
	lua_setfield(L, -2, "__index");
}
