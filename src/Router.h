#ifndef ROUTER_H
#define ROUTER_H

#include <map>
#include <string>
#include <iostream>
#include <filesystem>

namespace fs = std;



class App;

class Router {
    App *app;
    // std::map<std::string,std::string> routes;
    std::map<std::string,std::string> routes;
public:
  Router(App *app);
  
  //   /// @brief Add a new api route to the server
  //   /// @param name Lua Module name for the route
  //   /// @param filename Path to the lua script containing the route
  // void addApiRoute(std::string name, std::string filename);
  /// @brief Add a new api route to the server
  /// @param route Route to the endpoint
    /// @param name Lua Module name for the route
    /// @param filename Path to the lua script containing the route
  void addRoute(std::string route,std::string name, std::string filename);

  std::string getEndpoint(std::string endpoint);

};

#endif /* ROUTER_H */
