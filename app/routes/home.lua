--[home.lua] /

function GET()
    --local name = request.query.name
    local params = {}
    if (request.query.name == nil) then
        params.name = "No name"
    else
        params.name = request.query.name .. " the third"
    end

    return renderTemplate("html/index.html", params), 200
end

function POST()
    local aTable = {
        name = "table",
        job = {
            title="a job"
        }
    }
    return jsonify(aTable), 200
end