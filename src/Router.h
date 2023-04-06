#ifndef ROUTER_H
#define ROUTER_H

#include <map>
#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_map>




class App;

typedef struct EndpointMatch{
  std::string path;
  std::string url;
  std::string endpoint;
  std::map<std::string, std::string> parameters;
  std::map<std::string, std::string> routeParameters;
} EndpointMatch;

class Router
{
  App *app;
  // std::map<std::string,std::string> routes;
  std::map<std::string, std::string> routes;
public:
  Router(App *app);
  void clear();

  /// @brief Add a new api route to the server
  /// @param route Route to the endpoint
    /// @param filename Path to the lua script containing the route
  void addRoute(std::string route, std::string filename);

    static std::map<std::string, std::string> parse_query_string(const std::string& query_string);
    static EndpointMatch  parseUrl(const std::string& url);
  EndpointMatch getEndpoint(std::string endpoint);
};

#endif /* ROUTER_H */
