--[hello.lua] /api/hello
api_hello = {}
name = "api.hello"

function api_hello.GET()
    response = {hello="world"}
    return jsonify(response),200
end

function api_hello.POST()
    return "{\"hello\":\"world\"}",200
end

