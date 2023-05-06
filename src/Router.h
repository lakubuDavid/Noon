#ifndef ROUTER_H
#define ROUTER_H

#include <map>
#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_map>




class App;

typedef struct EndpointMatch{
  // The path to the endpoint
  std::string path;
  // The full url
  std::string url;
  // The path to the lua script linked to the endpoint
  std::string endpoint;
  // For catch all routes : The path after the "catch all" part
  std::string subPath;
  // Query parameters
  std::map<std::string, std::string> parameters;
  // Route parameters for dynamic routes
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

    static std::map<std::string, std::string> parseQueryString(const std::string& query_string);
    static EndpointMatch  parseUrl(const std::string& url);
  EndpointMatch getEndpoint(std::string endpoint);
};

#endif /* ROUTER_H */
