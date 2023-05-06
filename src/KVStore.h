//
// Created by David Lakubu on 04/05/2023.
//

#ifndef KVSTORE_H
#define KVSTORE_H

#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>
#include <variant>

#include "ILuaExtension.h"
#include "RuntimeConfig.h"


#define conf  RuntimeConfig::instance()

static const std::string KVStoreRootPath =
        conf->get("NOON_KVSTORE_STORAGE_ROOT_PATH","./data/db/stores/")+
        conf->get("NOON_APP_NAME","no_name") + "." +
        conf->get("NOON_APP_VERSION","0.0") + "/";


enum KVStoreIndexType{
    KVS_INDEX_JSON_TYPE,
    KVS_INDEX_TEXT_TYPE
};
//
//template <class... _Types>
//class variant;

struct KVStoreIndex{
private:
    union{
        char* _string;
        nlohmann::json* _json;
    };
public:
    KVStoreIndexType type;

    std::string text();
    nlohmann::json json();

    std::variant<std::string,nlohmann::json> value();

    bool isNull();

    nlohmann::json to_json();
};



class KVStore {
    nlohmann::json* _json;
    std::string _name;

    std::map<std::string,KVStoreIndex> _indexes;

    KVStore(const std::string &name);

public:



    static KVStore* createStore(std::string name);
    static KVStore* getStore(std::string name);
    static KVStore* deleteStore(std::string name);
    static bool exists(std::string name);

    bool has(std::string key);
    KVStoreIndex get(std::string key,std::string defaultValue = "");
    void set(std::string key, std::string value);
    void remove(std::string key);

    void clear();
    std::map<std::string,KVStoreIndex> all();

    std::string operator[](std::string key);

    void save();
};

/* -- KVStore lua extension -- */

class KVStoreExtension : public ILuaExtension {
public:
    virtual void registerExtension(lua_State* L);
};

#endif //KVSTORE_H
