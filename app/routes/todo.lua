-- /todo -- 

local todo_store = KV.getStore("todo")

function GET()
    local todos = todo_store:all()
    return renderTemplate("html/todo.html", todos),200
end

function POST()

end

function PUT()
    
end

function DELETE()
    
end