#include "Application.h"
#include "MainWindow.h"
#include "test_TextRendererWindow.h"
#include "test_OGLWindow.h"
#include "debug.h"

#include <fstream>
#include <iostream>




int main(int argc, char ** argv)
{
  ENABLE_LEAK_DEBUG();

  try
  {
    std::ofstream ofs("log.txt");
    if (!ofs.is_open())
    {
      std::cerr << "Failed to create log file" << std::endl;
      return 1;
    }
    
    std::clog.rdbuf(ofs.rdbuf());
    
    Application & app = Application::instance();

    if (argc > 1)
    {
      app.setAssetsRootDir(argv[1]);
    }

    if (!app.init())
    {
      return 1;
    }
    
    //app.registerWindow(new Window("SDL Window manager", 800, 600));
    //app.registerWindow(new Window("Hello, World", 640, 480));

    app.registerWindow(new MainWindow("Fluid Simulator", 800, 600));
    //app.registerWindow(new TextRendererWindow("TextRendererWindow", 800, 600));
    //app.registerWindow(new OGLWindow("OGLWindow", 800, 600));

    app.run();
    
    //app.quit();
  }
  catch (std::exception & e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}