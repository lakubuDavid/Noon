//
// Created by David Lakubu on 12/04/2023.
//
#include "Dusk.h"
#include "DuskBackend.h"

using namespace std;

Result<std::string> Dusk::render(const std::string &name, lua_State *L) {
    Result<std::string> result;
    string backend_script,frontend_view;
    backend_script = "pages/"+name+".lua";
    frontend_view = "pages/"+name+".view.html";

    // Step 1: Execute the backend code
    Result<map<string,string> > backend_result = backend->run(backend_script, L);

    // Step 2: Render the frontend using data from the backend
    Result<string> frontend_result = frontend->render(frontend_view,backend_result.data,L);
    return result;
}

