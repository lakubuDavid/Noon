//
// Created by David Lakubu on 12/04/2023.
//

#ifndef DUSKVIEW_H
#define DUSKVIEW_H

#include <string>
#include <map>
#include <stack>
#include <lua.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Utils.h"
using namespace std;


enum DuskViewBlock{
    DUSK_BLOCK_UNKNOWN = -2,
    DUSK_BLOCK_FEEDING = -1,
    DUSK_BLOCK_STATEMENT,

    DUSK_BLOCK_IF_BLOCK,
    DUSK_BLOCK_ELSE_BLOCK,

};

class DuskView {
    std::stack<std::string> errorStack;
    Result<string> parseBlock(std::string html,int block_begin);
public:
    Result<string> render(const std::string &path, std::map<string, string> params, lua_State *L);
};


#endif //DUSKVIEW_H
