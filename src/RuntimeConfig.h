//
// Created by David Lakubu on 20/04/2023.
//

#ifndef RUNTIMECONFIG_H
#define RUNTIMECONFIG_H

#include <map>
#include <string>
#include <fstream>

class RuntimeConfig {
    std::map<std::string, std::string> _contextVars;
    static RuntimeConfig *_instance;

    RuntimeConfig();

public:
    static RuntimeConfig *instance();

    void set(std::string key, std::string value);

    std::string get(std::string key);
    std::string get(std::string key,std::string defaultValue);

    void loadEnv();

    std::string operator[](std::string key);

    bool has(std::string key);
    bool is(std::string key);
};

#endif //RUNTIMECONFIG_H
