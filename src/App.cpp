#include "App.h"


App::App()
{
    scriptEngine = new ScriptEngine();
    auto app = this;
    httpServer = new HttpServer(8080,this);
}
App::App(int port) : _port(port){
    scriptEngine = new ScriptEngine();
    httpServer = new HttpServer(this->_port,this);
}

App::~App()
{
    // Shutdown the script engine
    scriptEngine->close(); 
}

int App::run() {
    scriptEngine->init();
    // Open the noon.config.lua file to read the configuration for the application
    if(scriptEngine->loadModule("./noon.config.lua")){
        std::cout << "Loading configuration" << std::endl;
        std::cout << "\n---" << std::endl;
        std::cout << "App Name : " << scriptEngine->getString("appName") << std::endl;
        std::cout << "version : " << scriptEngine->getString("version") << std::endl;
        std::cout << "---" << std::endl;

        std::cout << "\n Initializing routes" << std::endl;

        auto routes = scriptEngine->getTable("routes");

        for(auto it = routes.begin(); it != routes.end();it++){
            auto key = it->first;
            auto path = it->second;
            
            auto name = path.substr(0, path.length() - 4);
            for (auto i = 0; i<name.length();i++){
                if(name[i] == '/')
                    name[i] = '_';
            }

            this->httpServer->router()->addRoute(key,name, path);
            // std::cout << "\t" << key << " ("<< name <<"): /" << path << std::endl;
            // this->httpServer->router()->addRoute("home", "api/home.lua");
        }
        // Initialize the http server
        httpServer->init();
        auto running = true;
        // Run the main loop
        std::cout <<"Listening http://localhost:"<<this->_port<< std::endl;
        while (running)
        {
            running = httpServer->tick();
        }
    }
    else{
        std::cout << "Can't load configuration (noon.config.lua not found)" << std::endl;
    }
    return 0;
}

ScriptEngine *App::script() { return scriptEngine; }
void App::setDevMode(bool devMode) {
  this->_devMode = devMode;
}