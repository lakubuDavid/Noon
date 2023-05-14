//
// Created by David Lakubu on 04/05/2023.
//

#ifndef KVSTORE_H
#define KVSTORE_H

#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>
#include <variant>
#include <exception>

#include "interfaces/ILuaExtension.h"
#include "interfaces/ISerializable.h"

#include "RuntimeConfig.h"
#include "Utils.h"

#define noon_conf RuntimeConfig::instance()

const std::string KVStoreRootPath =
		noon_conf->get("NOON_KVSTORE_STORAGE_ROOT_PATH", "./data/db/stores/");

enum KVStoreIndexType {
	KVS_INDEX_TYPE_JSON,
	KVS_INDEX_TYPE_TEXT,
	KVS_INDEX_TYPE_NUMBER,
	KVS_INDEX_TYPE_BOOLEAN,
	KVS_INDEX_TYPE_NULL
};
//
// template <class... _Types>
// class variant;

class KVStoreIndex : public ISerializable {
private:
	std::string _innerValue;

	KVStoreIndexType _type = KVS_INDEX_TYPE_NULL;
public:
	KVStoreIndex();

	KVStoreIndex(std::string value, KVStoreIndexType type);

	inline float boolean() {
		if (isBoolean()) {
			if (_innerValue == "true")
				return true;
			else if (_innerValue == "false")
				return false;
			else
				throw;
		}
		throw;
	}

	inline float number() {
		return isNumber() ? to_float(_innerValue) : throw;
	}

	inline std::string text() {
		return isText() ? _innerValue : throw;
	}

	inline nlohmann::json json() {
		return isJson() ? nlohmann::json::parse(_innerValue)
		                : throw;
	}

	inline KVStoreIndexType type() const { return _type; }

	inline bool isNull() {
		return type() == KVS_INDEX_TYPE_NULL;
	};

	inline bool isText() {
		return type() == KVS_INDEX_TYPE_TEXT;
	};

	inline bool isNumber() {
		return type() == KVS_INDEX_TYPE_NUMBER;
	}

	inline bool isBoolean() {
		return type() == KVS_INDEX_TYPE_BOOLEAN;
	}

	inline bool isJson() {
		return type() == KVS_INDEX_TYPE_JSON;
	}

	virtual nlohmann::json serialize() override;

	void deserialize(nlohmann::json serialized) override;
};

class KVStore : public ISerializable {

	std::string _path;
	nlohmann::json _json;
	std::string _name;
	bool _persist = true;

	std::map<std::string, KVStoreIndex> _indexes;


public:
	KVStore(const std::string &name, bool persist = true);

	~KVStore();

	static KVStore *getStore(std::string name);

	void deleteStore();

	static bool exists(std::string name);

	bool has(std::string key);

	KVStoreIndex get(std::string key);

	void set(std::string key, KVStoreIndex value);

	void remove(std::string key);

	void clear();

	std::map<std::string, KVStoreIndex> all();

	nlohmann::json serialize() override;

	void deserialize(nlohmann::json serialized) override;

	void flush();
};

/* -- KVStore lua extension -- */

class KVStoreExtension : public ILuaExtension {
public:
	virtual void registerExtension(lua_State *L);
//	virtual void registerExtension(ScriptContext *s_context);
};

class KVStoreIndexExtension : public ILuaExtension {
public:
	virtual void registerExtension(lua_State *L);
//	virtual void registerExtension(ScriptContext *s_context);
};

#endif // KVSTORE_H
