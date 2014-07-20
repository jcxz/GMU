/*
 * Copyright (C) 2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:

 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

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