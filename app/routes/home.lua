--[home.lua] /

home = {}
name = "hello"

function home.GET()
    --local name = request.query.name
    --local params =
    return renderTemplate("html/index.html",request.query), 200
end

function home.POST()
    return "", 200
end