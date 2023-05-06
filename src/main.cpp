#include <stdio.h>
#include <signal.h>
#include <string>
#include <fstream>
#include <csignal>
#include <boost/filesystem.hpp>

#include "App.h"
#include "RuntimeConfig.h"
#include "Log.h"
#include "Utils.h"

using namespace std;
App *app;

void createNewProject(string projectName, string rootPath) {
    if (!boost::filesystem::exists(rootPath)) {
        boost::filesystem::create_directory(rootPath);
    }
    ofstream f(rootPath + "/noon.config.lua");
    if (f.is_open()) {
        f << "-- Noon v1.0" << endl;
        f << "-- Config file for the app" << endl;

        f << "appName = \"" << projectName << "\"" << endl;
        f << "version = 1.0" << endl;
        f << "routes = {}" << endl;
        f.close();
    } else {
        cerr << "Can't create a project here" << endl;
    }
}

// Returns true if we can continue or false if we should not
// e.g. With --help  we don't need to execute the server ,so we return false
bool checkArguments(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
#ifdef DEBUG
        cout << "arg [" << i << "] " << argv[i] << endl;
#endif
    }
    for (int i = 1; i < argc; i++) {
        string arg = string(argv[i]);
        if (i == 1 && arg == "--help") {
            printHelp();
            return false;
        } else if (i == 1 && arg == "--version") {
            return false;
        } else if (i == 1 && arg == "new") {
            if (argc < 4) {
                cout << " (error) Incomplete call to new" << endl;
                printHelp();
            }
            auto name = argv[2];
            auto path = argv[3];
            createNewProject(name, path);
            return false;
        }
        if (arg == "--port" && i + 1 < argc) {
            i++;
            RuntimeConfig::instance()->set("NOON_SERVER_PORT", argv[i]);
        } else if (arg == "--appDir" && i + 1 < argc) {
            i++;
            RuntimeConfig::instance()->set("NOON_APP_DIR", argv[i]);
        } else if (arg == "--cert" && i + 2 < argc) {
            i++;
            RuntimeConfig::instance()->set("SSL_CERTIFICATE", argv[i]);
            i++;
            RuntimeConfig::instance()->set("SSL_CERTIFICATE_KEY", argv[i]);
        } else if (arg == "--debug") {
            RuntimeConfig::instance()->set("NOON_DEBUG", "1");
        }
    }
    return true;
}

void terminate(int signalNum) {
    std::cout << "Terminating the server (code: " << signalNum << ")" << std::endl;
    app->close();
    exit(signalNum);
}

int main(int argc, char **argv) {

//    signal(SIGKILL, terminate);
    signal(SIGINT, terminate);
//    signal(SIGTERM, terminate);
    RuntimeConfig::instance()->loadEnv();
    if (checkArguments(argc, argv)) {
        Log::init();
        app = new App();
        return app->run();
    }
    return 0;
}
