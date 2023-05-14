let count = 0
const todoList = document.querySelector("#todo-list")
const newTask = document.querySelector("#taskText")

async function loadData() {
    const response = await fetch("http://localhost:8080/api/todo")
    todoList.innerHTML = ""
    const todos = await response.json()
    // console.log(todos)
    for (const key in todos) {
        const todo = todos[key]
        let todoItem = document.createElement("li")
        todoItem.innerText = todo.text
        if (todo.done) {
            todoItem.classList.add("done")
            todoItem.innerText += "✅"
        }
        todoList.appendChild(todoItem)
    }
}

loadData()
document.querySelector("#add-task-btn").addEventListener("click", async (_ev) => {
    console.log(`adding task ${newTask.innerText}`)
    const response = await fetch("http://localhost:8080/api/todo", {
        method: "POST",
        body: JSON.stringify({task:newTask.innerText})
    })
    
    loadData()
})