#include "Router.h"
#include "App.h"
#include "Log.h"

using namespace std;

Router::Router(App *app) { this->app = app; }

void Router::addRoute(std::string route, std::string filename) {

//    std::stringstream out;
//    out << "Adding route " << route
//              << "\n\tpath:" << filename << std::endl;
//    Log::println(out.str());
	// 1. Load the lua script
//    auto script = this->app->script();
	routes.insert(std::make_pair(route, filename));


}

/*
   Synopsis: we have different types of routes:
   - static routes : The simple kind of routes will only work when totally
   matching
   - dynamic routes : These are the adaptive ones that will work when the static
   parts are matching and will passe the dynamic part as a parameter eg :
   /users/:username will accept /users/john or /users/mwima and will pass the
   usernames (john and mwima) as parameters
   TODO : Use regex to check if matching
   -- catch all routes : Will accept all routes that points to a part of it
       eg : /functions/__all__ will accept all routes that starts with
   /functions/ like /functions/math , /functions/math/algebra/sqrt ,... and will
   pass the rest as a sub route parameter like
 */
EndpointMatch Router::getEndpoint(std::string path) {
	EndpointMatch match = parseUrl(path);

	if (this->routes.find(match.path) != this->routes.end()) {
		match.endpoint = routes[match.path];
	} else if (boost::filesystem::exists(getPath("static/" + path))) {
		match.endpoint = path;
	} else {
		bool found = false;
		/* -- Check catch all routes -- */
		for (auto kvp: this->routes) {
			if (kvp.first.ends_with("...")) {
				int l = kvp.first.length() - 3;
				auto base_path = kvp.first.substr(0, l);
				if (path.starts_with(base_path)) {
					auto subPath = path.substr(l);

					match.endpoint = kvp.second;
					match.subPath = subPath;

					found = true;
					break;
				}
			}
		}
		/* -- Check dynamic routes -- */
		// TODO : Check for dynamic routes
		if (!found) {
			match.endpoint = "404";
		}

	}
	return match;
}

std::map<std::string, std::string> Router::parseQueryString(const std::string &query_string) {
	std::map<std::string, std::string> query_params;
	size_t pos = 0;
	while (pos < query_string.length()) {
		size_t key_start = pos;
		size_t key_end = query_string.find('=', key_start);
		if (key_end == std::string::npos) {
			break;
		}
		size_t value_start = key_end + 1;
		size_t value_end = query_string.find('&', value_start);
		if (value_end == std::string::npos) {
			value_end = query_string.length();
		}
		std::string key = query_string.substr(key_start, key_end - key_start);
		std::string value = query_string.substr(value_start, value_end - value_start);
		for (int i = 0; i < value.length(); i++) {
			if (value[i] == '+')
				value[i] = ' ';
		}
		query_params[key] = value;
		pos = value_end + 1;
	}
	return query_params;
}

EndpointMatch Router::parseUrl(const string &url) {
	EndpointMatch match;
	auto pos = url.find('?');
	auto main_url = url.substr(0, pos);
	auto params = parseQueryString(url.substr(pos + 1));
	match.url = url;
	match.path = main_url;
	match.parameters = params;
	return match;
}

void Router::clear() {
	routes.clear();
}
