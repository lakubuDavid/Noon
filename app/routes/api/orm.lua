-- api/todo.lua --- 

useMiddleware("db")
----------------------------- ORM SETTINGS --------------------------------

DB = {
    DEBUG = false,
    new = true,
    backtrace = true,
    name = "database.db",
    type = "sqlite3",
}

----------------------------- REQUIRE --------------------------------

local Table = require("orm.model")
local fields = require("orm.tools.fields")
----------------------------- CREATE TABLE --------------------------------

Todo = Table({
    __tablename__ = "todo",
    -- id = fields.IntegerField({unique=true}),
    text = fields.TextField({max_length = 256}),
    done = fields.BooleandField({default = false})
})
----------------------------------------------------------------
function GET()
    if not (request.query.id == nil )then
        return jsonify(Todo.get:where({id=request.query.id}))
    end
    return jsonify(Todo.get:all())
end

function POST()
    if not (request.body == nil) then
        local task = Todo({
            text = request.body.task,
            done = false
        })
    end
end

function DELETE()
    
end
