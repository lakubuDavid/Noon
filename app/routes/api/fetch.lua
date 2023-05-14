
function GET()
    -- local data = fetch("http://127.0.0.1:4999","GET",{})
    local data = fetch("https://jsonplaceholder.cypress.io/todos/1","GET",{})
    -- local data = fetch("https://localhost:4000/api/users","GET",{})
    local status = 200
    local response
    if(data == nil) then
        status = 404
        response = "No data available"
    end
    response = data
    return response,status
end
