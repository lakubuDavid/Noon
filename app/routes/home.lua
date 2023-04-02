--[home.lua] /

home = {}
name = "hello"

function home.GET() 
    return renderTemplate("html/index.html"), 201 
end

function home.POST()
    return "", 200
end
