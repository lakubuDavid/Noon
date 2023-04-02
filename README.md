
# Noon : A Lua web server/framework

## Synopsis

*Due to a mix of curiosity and boredom, I asked mywself "Can I make a website in C?" but as I could only setup a C++ project I decided to go for it and to accelerate the development time I decided to add Lua to accelerate it.*

*So that's what I did,sacrificing part of my time,sleep and mental health to achive this weird objective using as language that only an elite that has sold itself to the devil can pretend to master 20% of it.*

## Description

Noon is a web server that uses Lua as it's main language for handling requests and sending responses.
It doesn't use any tenplating engine so everything is done in good 'ole HTML.

## Roadmap

Here are the main steps of the projects that I will be implementing to consider it a "fullish" web server/framework :

- Returning HTML page ***(done)*** ✅
- Returning JSON objects ***(done)*** ✅
- Handling requests body/form ***(not implemented)*** ❌
- Handling requests cookies ***(not implemented)*** ❌
- Handling requests sessions ***(not implemented)*** ❌
- Add middlewares ***(not implemented)*** ❌
- Have  a full routing system ***(partially | in progress)*** ✔️👷🏾
  - Static routes   ***(done)*** ✅
  - Parameters ***(not implemented)*** ❌
  - Dynamic routes ***(not implemented)*** ❌
- Fetching data from the web ***(not implemented | in progress)*** ❌👷🏾
- Serving static files  ***(not implemented | in progress)*** ❌👷🏾
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
    static/         -- Contains all the static files (not implemented)
```


### noon.config.lua

The `noon.config.lua` file looks like this :

```lua
-- noon.config.lua

appName= "MyApp"
version= "1.0"
routes= {
    ["/"] = "index.lua"                     -- / points to /routes/index.lua 
    ["/api/hello.lua"] = "api/hello.lua"    --/api/hello points to /routes/api/hello.lua 
    ...
}
```

### Creating a route

Routes are defined in the routes folder, as lua tables that have the same name as the full file path (from the routes directory) with the `'/'` replaced by `'_'`

> eg: `routes/api/auth/login.lua` will define a table called `api_auth_login`

This table will contain functions corresponding to the request method.The function must returns the response (html or json) and the status code

```lua
-- index.lua

index = {}

function index.GET()
    return renderTemplate("/html/index.html"),200
end

function index.POST()
    ...
end

...
-- /api/hello.lua

api_hello = {}

function api_hello.GET()
    local data = {
        firstName = "John",
        lastName = "Doe"
    }

    return jsonify(data),200
end

function api_hello.POST()
    ...
end

```

## Limitations

Since static file serving is not yet implemented you can't reference assets from your project that includes `css,js` files and **pictures**

>### Workaround
>
>For `css` and `js` files you can have them in your html file.
>
>You can also reference your assets from a cdn.
## Development (pls help me 😫)

I'm currently trying to implement data fetching and static file serving but I can't figure out how to implement it. So to whoever has enough knowledge of C++, please feel free to help.

*Yours faithfully,*<br>
*A guy that has decided to lose his sanity to a language he already gave up on a few years ago.*

## Build requirements

Here is a list of third party requirements that you may need to build it:

- Lua 5.4
- Boost 1.81

## Contact me

For any information you can contact me at [pro.lakubudavid@gmail.com](mailto:pro.lakubudavid@gmail.com)

>*It is normal for me to take 2 days to read my emails and 2 more days to reflect on the matter and respond calmly. The culture of immediacy and the constant fragmentation of time are not very compatible with the kind of life I lead.*
