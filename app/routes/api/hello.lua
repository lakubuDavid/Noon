--[hello.lua] /api/hello

function GET()
    response = {hello="world"}
    return jsonify(response),200
end

function POST()
    return "{\"hello\":\"world\"}",200
end

