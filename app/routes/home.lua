--[home.lua] /
function GET()
    --local name = request.query.name
    local params = {name="Mathié",phrase="Je suis où?"}
    return renderTemplate("html/index.html", params), 200
end
