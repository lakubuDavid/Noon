//
// Created by David Lakubu on 04/05/2023.
//

#ifndef ILUAEXTENSION_H
#define ILUAEXTENSION_H

#include <lua.hpp>

class ILuaExtension {
public:
	virtual void registerExtension(lua_State *L) = 0;
};


#endif //ILUAEXTENSION_H
