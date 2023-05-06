//
// Created by David Lakubu on 04/05/2023.
//

#include "KVStore.h"

KVStore::KVStore(const std::string& name) {
    this->_json = new nlohmann::json();
    this->_name = name;
}

KVStore *KVStore::createStore(std::string name) {
    auto store = new KVStore(name);
    return store;
}

KVStore *KVStore::getStore(std::string name) {
    auto path = KVStoreRootPath + "/" + name + ".json";
//    if(boost::filesystem::exists(path)){
//        auto kvs = new KVStore(name);
//        auto f = std::ifstream(path);
//    }
//    else {
        return createStore(name);
//    }
}

KVStore *KVStore::deleteStore(std::string name) {
    return NULL;
}

bool KVStore::exists(std::string name) {
    auto path = KVStoreRootPath + "/" + name + ".json";
    return boost::filesystem::exists(path);
}

std::string KVStoreIndex::text() { return type == KVS_INDEX_TEXT_TYPE ? _string : ""; }

nlohmann::json KVStoreIndex::json() { return type == KVS_INDEX_JSON_TYPE ? *_json : nlohmann::json(); }
