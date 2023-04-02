#include <stdio.h>

#include "App.h"

int main(int argc, char** argv){
  App app;
  app.setDevMode(true);
  return app.run();
}