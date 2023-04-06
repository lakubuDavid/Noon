
# Notes

## [Wednesday 5,April 2023]

### Middlewares

So, to implement middleware first I will need to specify that a route is using a given middleware

#### Idea

I will call a function at the top of the route file to say "Use this middleware"

```lua
-- /routes/profile.lua

useMiddleware("checkLoggedIn")

function GET()
    ...
end

-- /middlewares/checkLoggedIn.lua

function handle()
    local user = someCodeToGetUser()
    if(user.loggedIn) then
        request.user = user
    end
    return next()
end
```
>**NOTE**: maybe I'll change the name of the middleware function 


#### Middleware return value

The middleware function also need to return something to redirect or block a request
with a set of functions

```lua

    next() -- Continue the request execution flow as it is
    abort(status_code) -- Abort the request execution flow and returns the status code (403 by default)
    redirect(path) -- Redirect the request to path
```



#### Application

Middlewares will be registered as the useMiddleware function is executed
so between the ```lua_dofile(L,routes)``` and  the execution of the routes.
So we need a way of keeping a middleware list in memory before executing the route

##### Idea 1

We define a global variable that will be used to store the middleware list <br>
Then before we execute the routes we access that list of middlewares,execute each one of them and 
then we execute route.

**POSSIBLE ISSUE** : The middleware `handle` functions will override each other since we need the same context.
As they will be executed one after the other,it should not pose a problem since the previous definition of `handle`
won't be necessary anymore.

## [Thursday 6,April 2023]

### Configs hot reloading

While implementing middlewares, I have decided to also add hot reload of the 
`noon.config.lua` file ,since every script are loaded on execution ,it means that they
can be modified while executing but for the `noon.config.lua` file it's more complicated.
Since routes and app configs are in that file,we need to update the router and apps configs
as soon as the script is modified because thats the only file that is called only one, on startup.

## Problem #1 : Multi threading

So, the initial watcher was supposed to be in a separate thread but whenever I try to join it,
it blocks the entire app

## Middleware function rename
The middleware function has been renamed to `RouteHandler`to keep it recognizable since 
it's a global function

## Middlewares Parameters

Some middlewares might need parameters to avoid having to rewrite similar code.<br>
For example a middleware that only allows user with a certain role

```lua
-- /middlewares/checkRole
function RouteHandler(params)
    -- Fetch the user
    local user = db.users:get(request.body.id)
    -- If the user has the correct role
    if(user.role == params.role) then
        return next()
    else
        if(params.redirect_url != nil) then
            return redirect(params.redirect_url)
        else
            return abort(403)
        end
    end
end 
```

## Data fetching

Since I'm finally done with middlewares I would like to work on two of the most useful
features : **Data fetching** and **static file serving**.
