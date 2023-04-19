#include <stdio.h>
#include <signal.h>
#include "App.h"


int main(int argc, char **argv) {
//    signal(SIG)
    App app;
//  app.setDebugMode(true);
    return app.run();
}