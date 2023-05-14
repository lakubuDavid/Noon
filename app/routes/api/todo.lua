---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by davidlakubu.
--- DateTime: 12/05/2023 17:14
---

function GET()
    local store = KV.getStore("todo")

    -- print(store)
    local todos = store:all()
    -- print(todos)
    -- print("Todos fetched \n")
    return jsonify(todos), 200
end

function POST()
    if (not (request.body == nil) or not (request.form == nil)) then
        local store = KV.getStore("todo")
        local task = {
            text = request.body.task,
            done = false
        }
        store:set(string.format(math.random(9999)), task)
        store:save()
        return store:serialize(), 200
    end
    return jsonify({ error = "Incomplete request body" }), 400
end