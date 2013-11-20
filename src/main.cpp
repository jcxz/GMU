#include "Application.h"
#include "MainWindow.h"
#include "Logger.h"


#include <fstream>
#include <iostream>



struct MyClass
{
  int value;

  friend std::ostream & operator<<(std::ostream & os, const MyClass & cls)
  {
    return os << "MyClass: " << cls.value << std::endl;
  }
};


int main(int /* argc */, char ** /* argv */)
{
#if 0
  Logger logger;

  logger.logMessage("Hello, World", Logger::SEV_INFO);
  logger.getStreamProxy(Logger::SEV_INFO, Logger::DEST_ALL) << "Some super Text, " << "whoose value is: " << 123 << ", and some float: " << 32.5f << std::endl;

  MyClass my_cls;
  my_cls.value = 10;

  logger.getStreamProxy(Logger::SEV_INFO, Logger::DEST_CONSOLE) << my_cls;
#endif

  std::ofstream ofs("log.txt");
  if (!ofs.is_open())
  {
    std::cerr << "Failed to create log file" << std::endl;
    return 1;
  }

  std::clog.rdbuf(ofs.rdbuf());

  Application & app = Application::instance();

  if (!app.init())
  {
    return 1;
  }

  //app.registerWindow(new Window("SDL Window manager", 800, 600));
  //app.registerWindow(new Window("Hello, World", 640, 480));

  app.registerWindow(new MainWindow("Fluid Simulator", 800, 600));

  app.run();

  //app.quit();

  return 0;
}