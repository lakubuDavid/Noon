---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by davidlakubu.
--- DateTime: 05/04/2023 13:14
---

useMiddleware("hello",{name="Mark"})
useMiddleware("world")

function GET()
   local j = jsonify(___context)
   print(j)
   return jsonify({info="This route should show how middlewares works "})
end