//
// Created by David Lakubu on 12/04/2023.
//

#include "DuskView.h"

using namespace std;

Result<std::string> DuskView::render(const std::string &path, map<string, string> params, lua_State *L) {
    Result<string> result;
    bool err = false;
    // The cursor position
    long pos = 0;
    // True if I'm in the middle of a template string
    bool in_block = false;
    DuskViewBlock current_block = DUSK_BLOCK_FEEDING;
    DuskViewBlock next_block = DUSK_BLOCK_UNKNOWN;

    auto file = std::ifstream(path);
    if (!file.is_open()) {
        std::cerr << "Error opening " << path << std::endl;
        // throw;
        return Result<std::string>("");
    }
    auto html = std::stringstream();
    std::string line;

    while (std::getline(file, line)) {
        html << line << std::endl;
    }
    for (auto kvp: params) {

        lua_pushstring(L, kvp.second.c_str());
        lua_setglobal(L, kvp.first.c_str());
    }

    string html_s = html.str();

    // FIXME : Check if we are in a html block
    auto script_begin = html_s.find("<script>");
    auto script_end = html_s.find("</script>");

    string delimiter;
    // 1. Parse control blocks
    // TODO : Parse control blocks (if...else..end,for...in...end)
//    string controlled_html = html_s;
    // IF
    {
        long prev_pos;
        delimiter = "<%if";
        string expression;
        while (html_s.find(delimiter, pos) != string::npos && !err) {
            pos = html_s.find(delimiter, pos); // NOLINT(cppcoreguidelines-narrowing-conversions)
            prev_pos = pos;
            pos = html_s.find("%>",pos);
            expression = "return " + html_s.substr(prev_pos + 4, pos - prev_pos - 4);
            luaL_dostring(L, expression.c_str());
            if (!lua_isboolean(L, -1)) {
                cerr << "If expression must be boolean" << endl;
                err = true;
            }

        }

    }

    // 2. Parse expressions
    delimiter = "{{";
    pos = 0;
    while (html_s.find(delimiter, pos) != string::npos) {
        long prev_pos = pos;
        pos = html_s.find(delimiter, pos); // NOLINT(cppcoreguidelines-narrowing-conversions)
        // Alternate between limiters

        in_block = !in_block;
        if (in_block) {
            delimiter = "}}";
        } else {
            delimiter = "{{";

            // We have a block
            // Now it's time to use it
            string block = html_s.substr(prev_pos + 2, pos - prev_pos - 2);
            string result;
            if (luaL_dostring(L, ("return " + block).c_str()) == LUA_OK) {
                if (lua_isnil(L, -1)) {
                    result += "";
                } else if (lua_isboolean(L, -1)) {
                    result += lua_toboolean(L, -1) ? "true" : "false";
                } else if (lua_isnumber(L, -1)) {
                    result += std::to_string(lua_tonumber(L, -1));
                } else if (lua_isstring(L, -1)) {
                    result += lua_tostring(L, -1);
                } else {
                    result += "";
                }
                lua_pop(L, 1);
            } else {
                lua_Debug debug_info;
                lua_getstack(L, 1, &debug_info);
                lua_getinfo(L, "Sl", &debug_info);
                std::cerr << debug_info.short_src << ":" << debug_info.currentline
                          << " " << debug_info.what << endl;
                cerr << lua_tostring(L, -1) << endl;
                lua_pop(L, -1);
                result += "$$ERROR$$";
            }
            //Push the result in the html
            html_s.replace(prev_pos, pos - prev_pos + 2, result);
        }
    }

    // Cleanuo everything
    for (auto kvp: params) {
        lua_pushnil(L);
        lua_setglobal(L, kvp.first.c_str());
    }

    return result;
}
