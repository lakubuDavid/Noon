-- Config file for the app
appName = "Sample App"
version = "1.0"
routes = {
    ["/"] = "home.lua",
    ["/todo"] = "todo.lua",
    ["/api/todo"] = "api/todo.lua",
    ["/api/hello"] = "api/hello.lua",
    ["/api/fetch"] = "api/fetch.lua",
    ["/api/query"] = "api/query.lua",
    ["/api/post"] = "api/post.lua",
    ["/api/middlewares"] = "api/middlewares.lua",
    ["/api/orm"] = "api/orm.lua",
    ["/..."] = "_fallback.lua"
}
