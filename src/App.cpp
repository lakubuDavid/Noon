#include "App.h"


App* App::instance = NULL;

App::App(int port) : _port(port){
    instance = this;
    scriptEngine = new ScriptEngine(this);
    httpServer = new HttpServer(this->_port,this);
}

App::~App()
{
    // Shutdown the script engine
    scriptEngine->close(); 
}

bool App::loadConfig() {
    if(scriptEngine->loadModule("./noon.config.lua")) {
        std::cout << "Loading configuration" << std::endl;
        std::cout << "\n---" << std::endl;
        std::cout << "App Name : " << scriptEngine->getString("appName") << std::endl;
        std::cout << "version : " << scriptEngine->getString("version") << std::endl;
        std::cout << "---" << std::endl;
        std::cout << "\n Initializing routes" << std::endl;

        auto routes = scriptEngine->getTable("routes");
        for(auto & route : routes){
            auto key = route.first;
            auto path = route.second;

            this->httpServer->router()->addRoute(key, path);
            // std::cout << "\t" << key << " ("<< name <<"): /" << path << std::endl;
            // this->httpServer->router()->addRoute("home", "api/home.lua");
        }
        return true;
    }
    return false;
}

int App::run() {
    scriptEngine->init();
    // Open the noon.config.lua file to read the configuration for the application
    if(loadConfig()){

        scriptEngine->close();

        auto config = script()->watchFile("./noon.config.lua",App::onConfigChanged);


        // Initialize the http server
        httpServer->init();
        running = true;
//        config.watcher->join();
        // Run the main loop
        std::cout << "Listening on http://localhost:" << this->_port << std::endl;

        while(running){
            running = httpServer->tick();
        }
        httpServer->exit();
    }
    else{
        std::cout << "Can't load configuration (noon.config.lua not found)" << std::endl;
    }
    return 0;
}

ScriptEngine *App::script() { return scriptEngine; }

void App::setDebugMode(bool devMode) {
  this->_debugMode = devMode;
}

void App::onConfigChanged(FileInfo info) {
    bool scriptWasOpened = App::instance->script()->isOpen();
    auto script = instance->script();
    if(!scriptWasOpened)
        script->close();
    script->init();
    {
        if(script->loadModule("./noon.config.lua"))
            instance->loadConfig();
        else
            instance->running = false;
    }
    script->close();
}


