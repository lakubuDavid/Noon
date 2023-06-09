-- /todo -- 


function GET()
    local store = KV.getStore("todo")
    local _todos = store:all()
    return renderTemplate("html/todo.html", {todos=jsonify(_todos)}), 200
end

function POST()
    if (not (request.body == nil)) then
        local store = KV.getStore("todo")
        local task = {
            text = request.body.task,
            done = false
        }
        store:set(string.format(math.random(9999)), task)
        store:save()
        return redirect("/"), 200
    end
    return jsonify({ error = "Incomplete request body" }), 400
end

function PUT()

end

function DELETE()

end