
# Noon : A Lua web server/framework

## Synopsis

*Due to a mix of curiosity and boredom, I asked my self "Can I make a website in C?" but as I could only set up a C++ project I decided to go for it and to accelerate the development time I decided to add Lua.*

*So that's what I did,sacrificing part of my time,sleep and mental health to achieve this weird objective using as language that only an elite that has sold itself to the devil can pretend to master 20% of it.*

## Description

Noon is a web server that uses Lua as it's main language for handling requests and sending responses.
It doesn't use any tenplating engine so everything is done in good 'ole HTML.

## Roadmap

Here are the main steps of the projects that I will be implementing to consider it a "fullish" web server/framework :

- Returning HTML page ***(done)*** ✅
- Returning JSON objects ***(done)*** ✅
- Handling requests body/form ***(done)*** ✅
- Handling requests cookies ***(not implemented)*** ❌
- Handling requests sessions ***(not implemented)*** ❌
- Add middlewares ***(done)*** ✅
- Have  a full routing system ***(partially | in progress)*** 🟨️👷🏾
  - Static routes   ***(done)*** ✅
  - Parameters ***(not implemented)*** ❌
  - Dynamic routes ***(not implemented)*** ❌
- Fetching data from the web ***(not implemented | in progress)*** ❌👷🏾
- Serving static files  ***(partially implemented | in progress)*** 🟨 👷🏾
- **HTTPS Support** ***(not implemented)*** ❌
- ***Maybe*** add a templating engine ***(maybe)*** 🤷🏾‍♂️

## Using it

To have a basic working app you need at least a noon.config.lua and one route.

### Directory setup

The project is composed like this :

```lua
<app folder>/
    noon(.exe)      -- The server executable
    noon.config.lua -- The configuration file
    routes/         -- Contains the routes
    middlewares     -- Contains the middlewares
    static/         -- Contains all the static files
```


### noon.config.lua

The `noon.config.lua` file looks like this :

```lua
-- noon.config.lua

appName= "MyApp"
version= "1.0"
routes= {
    ["/"] = "index.lua"                     -- / points to /routes/index.lua 
    ["/api/hello"] = "api/hello.lua"    --/api/hello points to /routes/api/hello.lua 
    ...
}
```

### Creating a route \[UPDATED\]

Routes are defined in the routes folder as lua  scripts that contains functions corresponding to the http methods they respond to

```lua
-- index.lua

function GET()
    local params = {}
    return renderTemplate("/html/index.html",params),200
end

function POST()
    ...
end

...
-- /api/hello.lua

function GET()
    local data = {
        firstName = "John",
        lastName = "Doe"
    }

    return jsonify(data),200
end

function POST()
    ...
end

```
### Rendering html
I use a basic template engine that can only execute basic expressions or replace a variable.<br>
To use it just pass the arguments as a table to the `renderTemplate` function and in the html surround the expression with `{ }`.

```lua
-- index.lua
function GET()
    local params = {
      title="My page",
      name="Bob"
    }
    return renderTemplate("/html/index.html",params),200
end
```

```html
<!-- html/index.html -->
<html>
<head>
  <title>{{title}}</title>
</head>
<body>
    Hello, {{name}}
</body>
</html>
```

## Limitations

Since static file serving is not ~~implemented~~ complete, you can't reference non-text assets like images,videos or audios.

>### Workaround
>
>You can use pictures from a cdn

## Development (pls help me 😫)

I'm currently trying to implement data fetching and static file serving, but I can't figure out how to implement it. So to whoever has enough knowledge of C++, please feel free to help.

*Yours faithfully,*<br>
*A guy that has decided to lose his sanity to a language he already gave up on a few years ago.*

## Build requirements

Here is a list all the libraries that you will need to build it:

- [Lua 5.4](https://www.lua.org/download.html)
- [Boost 1.81](https://www.boost.org)
- [Openssl 3.1](https://www.openssl.org/source/)
- [JSON for modern C++](https://github.com/nlohmann/json)

## Contact me

For any information you can contact me at [pro.lakubudavid@gmail.com](mailto:pro.lakubudavid@gmail.com)

>*It is normal for me to take 2 days to read my emails and 2 more days to reflect on the matter and respond calmly. The culture of immediacy and the constant fragmentation of time are not very compatible with the kind of life I lead.*
