-- Config file for the app
appName = "Sample App"
version = "1.0"
routes = {
    ["/"] = "home.lua",
    ["/api/hello"] = "api/hello.lua",
    ["/api/fetch"] = "api/fetch.lua",
    ["/api/query"] = "api/query.lua",
    ["/api/post"] = "api/post.lua"
}