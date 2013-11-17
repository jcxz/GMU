#include "Application.h"
#include "Window.h"

#include <iostream>



int main(int /* argc */, char ** /* argv */)
{
  Application & app = Application::instance();

  if (!app.init())
  {
    return 1;
  }

  app.registerWindow(new Window("SDL Window manager", 800, 600));
  app.registerWindow(new Window("Hello, World", 640, 480));

  app.run();

  //app.quit();

  return 0;
}