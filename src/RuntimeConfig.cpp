//
// Created by David Lakubu on 20/04/2023.
//

#include "RuntimeConfig.h"

using namespace std;

RuntimeConfig *RuntimeConfig::_instance = NULL;

RuntimeConfig::RuntimeConfig() {
	_contextVars = std::map<std::string, std::string>();
}

RuntimeConfig *RuntimeConfig::instance() {
	if (_instance == NULL)
		_instance = new RuntimeConfig();
	return _instance;
}

void RuntimeConfig::set(std::string key, std::string value) {
	_contextVars[key] = value;
}

std::string RuntimeConfig::get(std::string key) {
	return _contextVars[key];
}

std::string RuntimeConfig::get(std::string key, std::string defaultValue) {
	if (has(key))
		return _contextVars[key];
	return defaultValue;
}

std::string RuntimeConfig::operator[](std::string key) {
	return _contextVars[key];
}

void RuntimeConfig::loadEnv() {
	auto env_file = std::ifstream(".env");
	if (env_file.is_open()) {
		std::string line;

		while (std::getline(env_file, line)) {
			if (line[0] == '#') {
				continue;
			}
			if (line.find('=') != std::string::npos) {
				int pos = line.find('=');
				string key = line.substr(pos);
				string value = line.substr(pos + 1, line.length());
				set(key, value);
			}
		}
		env_file.close();
	}
}

bool RuntimeConfig::has(std::string key) {
	return _contextVars.find(key) != _contextVars.end();
}

bool RuntimeConfig::is(std::string key) {
	return has(key) && get(key) == "1";
}
