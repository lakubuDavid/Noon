#include "Router.h"
#include "App.h"

using namespace std;

Router::Router(App *app) { this->app = app; }

// void Router::addApiRoute(std::string name, std::string filename)
// {
//     // 1. Load the lua script
//     auto script = this->app->script();
//     script->loadModule(filename);
//     // 2. Register the api route
//     std::string endpoint = "/"+ name;

//     for (auto i = 0; i<endpoint.length();i++){
//         if(endpoint[i] == '.')
//             endpoint[i] = '/';
//     }
//     routes.insert(std::make_pair(endpoint,name));
// }

void Router::addRoute(std::string route, std::string filename) {
    auto name = filename.substr(0, filename.length() - 4);
    for (auto i = 0; i < name.length(); i++) {
        if (name[i] == '/')
            name[i] = '_';
    }

    std::cout << "Adding route " << route << "\n\tname:" << name
              << "\n\tpath:" << filename << std::endl;
    // 1. Load the lua script
    auto script = this->app->script();
    if (script->loadModule("routes/" + filename)) {
        // 2. Register the api route
        routes.insert(std::make_pair(route, name));
    } else {
        std::cout << "Can't load " << filename << std::endl;
    }
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
    EndpointMatch match;
    string ppath = path;
    int pos = path.find("%");
    if (pos!= -1)
    {
      ppath = path.substr(0, pos);
    }
    // TODO : Parse query parameters
    // TODO : Implement dynamic routes
    // TODO : Implement catch all routes

    if (this->routes.find(path) != this->routes.end()) {
        match.path = routes[path];
    } else {
        match.path = "404";
    }
    return match;
}