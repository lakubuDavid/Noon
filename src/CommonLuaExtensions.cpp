#include "CommonLuaExtensions.h"
#include <cstdio>
#include "RuntimeConfig.h"

using namespace std;

std::string nativeRenderTemplate(const std::string &path, map<string, string> params, lua_State *L) {
	auto file = std::ifstream(getPath(path));
	if (!file.is_open()) {
		std::cerr << "Error opening " << path << std::endl;
		// throw;
		return "";
	}
	auto html = std::stringstream();
	std::string line;

	while (std::getline(file, line)) {
		html << line << std::endl;
	}
	for (auto [key, value]: params) {
		lua_pushstring(L, value.c_str());
		lua_setglobal(L, key.c_str());
	}

	string html_s = html.str();
	long pos = 0;
	// True if I'm in the middle of a template string
	bool in_block = false;
	// FIXME : Check if we are in a html block
	auto script_begin = html_s.find("<script>");
	auto script_end = html_s.find("</script>");

	string limiter;
	// 1. Parse control blocks
	// TODO : Parse control blocks (if...else..end,for...in...end)
	// 2. Parse expressions
	limiter = "{{";
	while (html_s.find(limiter, pos) != string::npos) {
		long prev_pos = pos;
		pos = html_s.find(limiter, pos); // NOLINT(cppcoreguidelines-narrowing-conversions)
		// Alternate between limiters

		in_block = !in_block;
		if (in_block) {
			limiter = "}}";
		} else {
			limiter = "{{";
//            if((script_begin!=string::npos && prev_pos>script_begin && script_end!=string::npos && pos<script_end)||(script_begin!=string::npos && prev_pos>script_begin)){
//                continue;
//            }
			// We have a block
			// Now it's time to use it
			string block = html_s.substr(prev_pos + 2, pos - prev_pos - 2);
			string result;
			if (luaL_dostring(L, ("return " + block).c_str()) == LUA_OK) {
				if (lua_isnil(L, -1)) {
					result += "";
				} else if (lua_isboolean(L, -1)) {
					result += lua_toboolean(L, -1) ? "true" : "false";
				} else if (lua_isnumber(L, -1)) {
					result += std::to_string(lua_tonumber(L, -1));
				} else if (lua_isstring(L, -1)) {
					result += lua_tostring(L, -1);
				} else {
					result += "";
				}
				lua_pop(L, 1);
			} else {
				lua_Debug debug_info;
				lua_getstack(L, 1, &debug_info);
				lua_getinfo(L, "Sl", &debug_info);
				std::cerr << debug_info.short_src << ":" << debug_info.currentline
				          << " " << debug_info.what << endl;
				cerr << lua_tostring(L, -1) << endl;
				lua_pop(L, -1);
				result += "$$ERROR$$";
			}
			//Push the result in the html
			html_s.replace(prev_pos, pos - prev_pos + 2, result);
		}
	}

	// Cleanup everything
	for (auto [key, value]: params) {
		lua_pushnil(L);
		lua_setglobal(L, key.c_str());
	}
	return html_s;
}

int renderTemplate(lua_State *L) {
	if (lua_isstring(L, -1) || (lua_isstring(L, -2)) && lua_istable(L, -1)) {

		string path;
		map<string, string> params;
		if (lua_isstring(L, -1))
			path = lua_tostring(L, -1);
		else if (lua_istable(L, -1)) {
			path = lua_tostring(L, -2);
			lua_pushnil(L);
			while (lua_next(L, -2) != 0) {
				std::string key = lua_tostring(L, -2);
				std::string value = lua_tostring(L, -1);
				params[key] = value;
				lua_pop(L, 1);
			}
		}

		auto html = nativeRenderTemplate(path, params, L);
		lua_pushstring(L, html.c_str());
	} else {
		lua_Debug debug_info;
		lua_getstack(L, 1, &debug_info);
		lua_getinfo(L, "Sl", &debug_info);
		std::cerr << debug_info.short_src << ":" << debug_info.currentline
		          << " Expected a string as an input or a string with a table" << std::endl;
		return 0;
	}
	return 1;
}

char *nativeServeStatic(const std::string &path) {
	std::ifstream file;
	file.open(getPath(path));
	if (!file.is_open()) {
		std::cerr << "Error opening " << path << std::endl;
		// throw;
		return (char *) "";
	}
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	char *buffer = (char *) malloc(size);
	// if (!file.read(buffer, size)) {
	//   std::cerr << "Error reading data "<<path << std::endl;
	//     throw;
	// }
	std::string line;
	while (std::getline(file, line)) {
		strcat(buffer, line.c_str());
	}
	file.close();
	return buffer;
}

int serveStatic(lua_State *L) {
	if (lua_isstring(L, -1)) {
		auto path = lua_tostring(L, -1);
		auto ret = nativeServeStatic(path);
		lua_pushstring(L, ret);
		return 1;
	}
	return 0;
}

void jsonify_table(lua_State *L, nlohmann::json &obj) {
	int type;
	const char *key;
	lua_pushnil(L);
	try {
		while (lua_next(L, -2) != 0) {
			key = "";
			type = lua_type(L, -1);
			if (lua_isstring(L, -2))
				key = lua_tostring(L, -2);
			else
				continue;

			switch (type) {
				case LUA_TSTRING: {
					obj[key] = (lua_tostring(L, -1));
					break;
				}
				case LUA_TNUMBER: {
					obj[key] = (lua_tonumber(L, -1));
					break;
				}
				case LUA_TBOOLEAN: {
					obj[key] = (lua_toboolean(L, -1));
					break;
				}
				case LUA_TTABLE: {
					nlohmann::json sub_obj;
					jsonify_table(L, sub_obj);
					obj[key] = sub_obj;
					break;
				}
				case LUA_TNIL: {
					obj[key] = NULL;
					break;
				}
			}
			lua_pop(L, 1);
		}
	}
	catch (exception ex) {
		stringstream error;
		error << "[lua - error]: " << ex.what();
		Log::println(error.str());
	}
}

int jsonify(lua_State *L) {
	luaL_checktype(L, -1, LUA_TTABLE);

	nlohmann::json obj;
	jsonify_table(L, obj);

	std::string json_str = obj.dump();
	lua_pushstring(L, json_str.c_str());

	return 1;
}

int json_to_lua(lua_State *L) {
	if (!lua_isstring(L, -1)) {
		return 0;
	}
	string json_str = lua_tostring(L, -1);
	// Parse the JSON string
	auto json = nlohmann::json::parse(json_str);
	lua_newtable(L);
	json_object_to_lua(L, json);
	return 1;
}

void json_array_to_lua(lua_State *L, nlohmann::json &json_array) {}

void json_object_to_lua(lua_State *L, nlohmann::json &json) {
	for (auto el: json.items()) {
		lua_pushstring(L, el.key().c_str());
		int i;
		switch (el.value().type()) {
			case nlohmann::json::value_t::boolean:
				lua_pushboolean(L, el.value());
				break;
			case nlohmann::json::value_t::number_integer:
				lua_pushinteger(L, el.value());
				break;
			case nlohmann::json::value_t::number_unsigned:
				lua_pushinteger(L, el.value());
				break;
			case nlohmann::json::value_t::number_float:
				lua_pushnumber(L, el.value());
				break;
			case nlohmann::json::value_t::string:
				lua_pushstring(L, el.value().get<std::string>().c_str());
				break;
			case nlohmann::json::value_t::array:
				lua_newtable(L);
				i = 1;
				for (auto sub_el: el.value().items()) {
					json_object_to_lua(L, sub_el.value());
					lua_rawseti(L, -2, i);
					i++;
				}
				break;
			case nlohmann::json::value_t::object:
				lua_newtable(L);
				for (auto sub_el: el.value().items()) {
					lua_pushstring(L, sub_el.key().c_str());
					json_object_to_lua(L, sub_el.value());
					lua_settable(L, -3);
				}
				break;
			case nlohmann::json::value_t::null:
				lua_pushnil(L);
				break;
			default:
				break;
		}
		lua_settable(L, -3);
	}
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

char *nativeFetch(std::string url, std::string method, std::string data, std::string headers) {
	int err;
	ConnectionInfo cinfo;
	cinfo = Connection::getInfo(url);
	string path;
	// Parse the request's path
	{
		// remove the protocol
		auto full_url = url;
		int pos = full_url.find("://");
		auto no_protocol = full_url.substr(pos + 3);
		pos = no_protocol.find("/");
		path = no_protocol.substr(pos);
	}
	printf("[fetch] : Fetching %s from %s | %s on port %d\n", path.c_str(), cinfo.host.c_str(),
	       method.c_str(), cinfo.porti);

	Connection *connection;

	if (cinfo.protocol == PROTOCOL_HTTP)
		connection = new SocketConnection(url);
	else
		connection = new SSLConnection(url);
	if (connection->open() < 0) {

		std::cerr << "Failed to open" << std::endl;
		ERR_print_errors_fp(stderr);
		return NULL;
	}


	string request;
	stringstream requestStream;
	requestStream << method + " " + path;
//    if (cinfo.protocol == PROTOCOL_HTTP)
//        requestStream << "HTTP/1.1\r\n";
//    else
	requestStream << " HTTP/1.1\r\n";
	requestStream << "Host: " + cinfo.host +
	                 "\r\n";
	requestStream << headers;
	requestStream << "Accept : */* \r\n";

	if (method == "POST" || method == "PUT") {
		requestStream << "Content-Type: "
		                 "application/x-www-form-urlencoded\r\nContent-Length: " +
		                 to_string(data.length()) + "\r\n";
		requestStream << "\r\n" + data;
	} else {
//        requestStream << "Content-Length : " << requestStream.str().length();
		requestStream << "\r\n";
	}
	request = requestStream.str();
	Log::println(request);

	try {
		if (connection->write((char *) request.c_str(), request.length()) < 0) {
			cerr << "[fetch] : Send failed" << endl;
			ERR_print_errors_fp(stderr);
			return NULL;
		}

		char buffer[1024] = {0};
		string response = "";
		while (connection->read(buffer, sizeof(buffer)) > 0) {
			response += buffer;
			memset(buffer, 0, sizeof(buffer));
		}

		connection->close();

		char *reslt = new char[response.length() + 1];
		strcpy(reslt, response.c_str());

		return reslt;
	} catch (const std::exception ex) {
		char *error;
		snprintf(error, sizeof(error), "[fetch] : Error: %s", ex.what());
		return error;
	}
}

#pragma clang diagnostic pop

int fetch(lua_State *L) {
	// old syntax : fetch(url,method,data,headers)
	// better syntax : fetch(url,method,options<should contain data and headers>)
	if (lua_isstring(L, -3) && lua_isstring(L, -2)) {
		try {
			string url = lua_tostring(L, -3);
			string method = lua_tostring(L, -2);

			Log::println("[lua] : fetching " + url + " | " + method);

			string data = "";
			string headers = "";

			/* -- Get the options table -- */
//            lua_pushnil(L);
			/*while (lua_next(L, -2) != 0) {
				auto key = lua_tostring(L, -2);
				printf("fetch options key : %s", key);
				if (key == "body" && lua_istable(L, -1)) {
					Log::println("[lua - fetch] : found request body");
					nlohmann::json obj;
					jsonify_table(L, obj);
					data = lua_tostring(L, -1);
				} else if (key == "headers" && lua_istable(L, -1)) {
					lua_pushnil(L);
					while (lua_next(L, -2) != 0) {
						std::string k = lua_tostring(L, -2);
						std::string v = lua_tostring(L, -1);
						headers += k + ":" + v + "\n";
						lua_pop(L, 1);
					}
				}
				lua_pop(L, 1);
			}
*/

			if ((method == "POST" || method == "PUT")) {
				// If the method is POST or PUT, the 3rd argument should be
				// provided as a string
				if (!lua_isstring(L, -1)) {
					lua_pushnil(L);
					return 1;
				} else {
					data = lua_tostring(L, -1);
				}
			}

			char *fdata = nativeFetch(url, method, data, headers);
			if (fdata == NULL) {
				lua_pushnil(L);
			} else {
				lua_pushstring(L, fdata);
			}
			return 1;
		}
		catch (std::exception &e) {
			printf("fetch error : %s\n", e.what());
			lua_pushnil(L);
			return 1;
		}
	}
	// First 2 args are mandatory
	printf("[lua] : Invalid arguments for fetch");
	lua_pushnil(L);
	return 1;
}

int getConfig(lua_State *L) {
	if (lua_isstring(L, -1)) {
		auto config = lua_tostring(L, -1);
		auto value = RuntimeConfig::instance()->get(config);
		lua_pushstring(L, value.c_str());
	} else {
		lua_pushstring(L, "");
	}
	return 1;
}

void CommonLuaExtension::registerExtension(lua_State *L) {
//	lua_pushcfunction(L, [](lua_State *L) -> int {
//		if (lua_isstring(L, -1) || (lua_isstring(L, -2)) && lua_istable(L, -1)) {
//			string path;
//			auto params = map<string, string>();
//			if (lua_isstring(L, -1))
//				path = lua_tostring(L, -1);
//			else if (lua_istable(L, -1)) {
//				path = lua_tostring(L, -2);
//				lua_pushnil(L);
//				while (lua_next(L, -2) != 0) {
//					std::string key = lua_tostring(L, -2);
//					std::string value = lua_tostring(L, -1);
//					params[key] = value;
//					lua_pop(L, 1);
//				}
//			}
//
//			auto html = nativeRenderTemplate(path, params, L);
//			lua_pushstring(L, html.c_str());
//		} else {
//			lua_Debug debug_info;
//			lua_getstack(L, 1, &debug_info);
//			lua_getinfo(L, "Sl", &debug_info);
//			std::cerr << debug_info.short_src << ":" << debug_info.currentline
//			          << " Expected a string as an input or a string with a table" << std::endl;
//			return 0;
//		}
//		return 1;
//	});
//	lua_setglobal(L, "renderTemplate");
}
