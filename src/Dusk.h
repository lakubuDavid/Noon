//
// Created by David Lakubu on 12/04/2023.
//

#ifndef DUSK_H
#define DUSK_H

/* =================================================================
 * Dusk Template Engine
 * Lakubu David
 * ---
 * A lua template engine
 * ----------------------------------------------------------------
 *
 * So here is the idea
 * A template engine that will be used for SSR Rendering
 * We will have two parts :
 * 1. The server side : that will execute on the server
 * 2. The client side : Just basic HTML and CSS with some special syntax
 *
 * The server side will be in a separate lua script
 * The client side will be in a '.view.html' file
 *
 * Data that will be passed to the client will be passed through a function that I will call 'export'
 *
 * No async data for now
 * */

#include <lua.hpp>
#include "DuskView.h"
#include "DuskBackend.h"

class Dusk{
    DuskView* frontend;
    DuskBackend* backend;
public:
    Result<std::string> render(const std::string& name,lua_State* L);
};

#endif //DUSK_H
