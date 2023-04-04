
function GET()
    local data = fetch("https://jsonplaceholder.typicode.com/todos/1","GET","")
    local status = 200
    local response
    if(data == nil) then
        status = 404
        response = "No data available"
    end
    response = data
    return response,status
end
