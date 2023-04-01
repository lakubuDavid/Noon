#include "Router.h"
#include "App.h"

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

void Router::addRoute(std::string route,std::string name, std::string filename) {
  std::cout<<"Adding route "<<route<<"\n\tname:"<<name<<"\n\tpath:"<<filename<<std::endl;
    // 1. Load the lua script
    auto script = this->app->script();
    if(script->loadModule("routes/"+filename)){
    // 2. Register the api route
    routes.insert(std::make_pair(route, name));
    } else {
      std::cout << "Can't load " << filename  << std::endl;
    }
}
std::string Router::getEndpoint(std::string path) {
  if (this->routes.find(path) != this->routes.end()) {
    return routes[path];
  } else {
    return "404";
  }
}