//
// Created by David Lakubu on 05/04/2023.
//

#ifndef MIDDLEWARES_H
#define MIDDLEWARES_H

#include <lua.hpp>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/any.hpp>
#include <iostream>
#include <map>

#include "ScriptContext.h"
#include "interfaces/ILuaExtension.h"

enum MiddlewareResult {
	MIDDLEWARE_RESULT_NEXT = 0,
	MIDDLEWARE_RESULT_ABORT = 1,
	MIDDLEWARE_RESULT_REDIRECT = 2
};


int middlewareContinue(lua_State *L);

int middlewareAbort(lua_State *L);

int middlewareRedirect(lua_State *L);

int useMiddleware(lua_State *L);

class MiddlewareExtension : public ILuaExtension {
public:
	virtual void registerExtension(lua_State *L);
//	virtual void registerExtension(ScriptContext *s_context) override;
};

#endif //MIDDLEWARES_H
