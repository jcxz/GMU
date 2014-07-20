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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <unordered_map>
#include <cstdint>


class Window;

class Application
{
  public:
    // returns singleton instance
    static Application & instance(void);

    void setAssetsRootDir(const char *assets_dir)
    {
      m_assets_dir = assets_dir;
    }

    const char *getAssetsRootDir(void) const
    {
      return m_assets_dir;
    }

    // registers a new window and takes ownership over it
    void registerWindow(Window *wnd);

    // unregisters window from application
    void unregisterWindow(Window *wnd);

    // This method initializes the application object
    bool init(void);

    // this method quits the application object
    void quit(void);

    // This function runs the event loop
    void run(void);

  private:
    // Application object is a singleton, which is not copyable
    Application(void)
      : //m_focused_window(nullptr),
        m_windows(),
        m_assets_dir("")
    {
    }

    ~Application(void)
    {
      // do not call quit here, cause it is causing some funkyness
      //quit();
    }

    Application(const Application & );
    Application & operator=(const Application & );

  private:
    void closeWindow(Window *wnd);

  private:
    // the first argument is windowID, the second one is the window itself
    typedef std::unordered_map<uint32_t, Window *> tWindowContainer;

  private:
    const char *m_assets_dir;    /// a path to directory containing assets (GL/CL sources, fonts, ...)
    //Window *m_focused_window;    /// the currently active window (this could in theory be infered from SDL event)
    tWindowContainer m_windows;  /// a list of application windows
};

#endif