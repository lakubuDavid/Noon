#include "App.h"
#include "RuntimeConfig.h"
#include "Log.h"


App *App::instance = NULL;

App::App() {
	if (RuntimeConfig::instance()->has("NOON_SERVER_PORT")) {
		_port = to_int(RuntimeConfig::instance()->get("NOON_SERVER_PORT"));
	}
	instance = this;
	scriptEngine = new ScriptContext(this);


	server = new Server(this->_port, this);
}

App::~App() {
	// Shutdown the script engine
	scriptEngine->close();
}

bool App::loadConfig() {
	auto config_path = getPath("noon.config.lua");
	if (scriptEngine->loadModule(config_path)) {
		std::cout << "Loading configuration" << std::endl;
		std::cout << "Initializing routes" << std::endl;

		auto routes = scriptEngine->getTable("routes");
		for (auto &route: routes) {
			auto key = route.first;
			auto path = route.second;
			Log::println("Added route : " + key + " : " + path);
			this->server->router()->addRoute(key, (path));
		}
		return true;
	}
	std::cout << "no config found" << std::endl;
	std::cout << "try setting the --appDir parameter to your app directory (the one with the `noon.config.lua` file)"
	          << std::endl;
	printHelp();
	return false;
}

int App::run() {

	scriptEngine->init();

	// Open the noon.config.lua file to read the configuration for the application
	if (loadConfig()) {

//        auto config = script()->watchFile("./noon.config.lua",App::onConfigChanged);

		// Initialize the http server
		server->init();
		running = true;
//        config.watcher->join();
		// Run the main loop
		RuntimeConfig::instance()->set("NOON_APP_NAME", scriptEngine->getString("appName"));
		RuntimeConfig::instance()->set("NOON_APP_VERSION", scriptEngine->getString("version"));
		std::cout << "———————————————————————————————————————————————————————————" << std::endl;
		std::cout << "  NOON Server v1.0a 🧪" << std::endl;
		std::cout << "     App Name    : " << scriptEngine->getString("appName") << std::endl;
		std::cout << "     App version : " << scriptEngine->getString("version") << std::endl;
		std::cout << "     Listening on http://" << server->getIpAddress() << ":" << this->_port << " ⚡️" << std::endl;
		std::cout << "———————————————————————————————————————————————————————————" << std::endl;

		scriptEngine->close();

		while (running) {
			running = server->listen();
		}
		server->exit();
	} else {
		std::cout << "Can't load configuration (noon.config.lua not found)" << std::endl;
	}
	return 0;
}

void App::close() {
	server->exit();
}

ScriptContext *App::script() { return scriptEngine; }

void App::setDebugMode(bool devMode) {
	this->_debugMode = devMode;
}

void App::onConfigChanged(FileInfo info) {
	bool scriptWasOpened = App::instance->script()->isOpen();
	auto script = instance->script();
	if (!scriptWasOpened)
		script->close();
	script->init();
	{
		if (script->loadModule(getPath("./noon.config.lua")))
			instance->loadConfig();
		else
			instance->running = false;
	}
	script->close();
}


