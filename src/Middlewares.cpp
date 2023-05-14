//
// Created by David Lakubu on 05/04/2023.
//

#include "Middlewares.h"
#include "CommonLuaExtensions.h"

int useMiddleware(lua_State *L) {
	std::string middleware;
	std::map<std::string, boost::any> params;
	if (lua_isstring(L, -1)) {
		middleware = lua_tostring(L, -1);
//        lua_pushnil(L);
//        lua_setglobal(L,"___middleware_params");
	} else if (lua_istable(L, -1)) {
		middleware = lua_tostring(L, -2);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "___middleware_params");
	}
	if (middleware.empty()) {
		std::cerr << "[lua] : A middleware must be specified" << std::endl;
		return 0;
	}
	std::string path = getPath("middlewares/" + middleware + ".lua");
	if (boost::filesystem::exists(path)) {
		lua_getglobal(L, "___context");
		{
			lua_getfield(L, -1, "middleware");
			{
				lua_getfield(L, -1, "stack");
				lua_pushstring(L, middleware.c_str());
				lua_pushstring(L, path.c_str());

				lua_rawset(L, -3);
			}
		}

	}
	return 0;
}

int middlewareContinue(lua_State *L) {
	lua_getglobal(L, "___context");
	{
		lua_getfield(L, -1, "middleware");
		{
			lua_getfield(L, -1, "stack");
			lua_remove(L, 1);
		}
	}
	lua_pushnumber(L, MIDDLEWARE_RESULT_NEXT);
	lua_pushnil(L);
	return 1;
}

int middlewareAbort(lua_State *L) {

	int status = 403;
	if (lua_isinteger(L, -1))
		status = lua_tointeger(L, -1);

	lua_pushnumber(L, MIDDLEWARE_RESULT_ABORT);
	lua_pushinteger(L, status);
	return 1;
}

int middlewareRedirect(lua_State *L) {
	std::string path = lua_tostring(L, -1);
	lua_pushnumber(L, MIDDLEWARE_RESULT_REDIRECT);
	lua_pushstring(L, path.c_str());
	return 1;
}

void MiddlewareExtension::registerExtension(lua_State *L) {
//	lua_State *L = s_context->getLuaState();
	lua_pushcfunction(L, useMiddleware);
	lua_setglobal(L, "useMiddleware");
 
	lua_pushcfunction(L, middlewareContinue);
	lua_setglobal(L, "continue");

	lua_pushcfunction(L, middlewareAbort);
	lua_setglobal(L, "abort");

	lua_pushcfunction(L, middlewareRedirect);
	lua_setglobal(L, "redirect");
}
