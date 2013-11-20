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

    void setAssetsDir(const char *assets_dir)
    {
      m_assets_dir = assets_dir;
    }

    const char *getAssetsDir(void) const
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
        m_assets_dir(nullptr)
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
    const char *m_assets_dir;    /// a path to directory containing assets
    //Window *m_focused_window;    /// the currently active window (this could in theory be infered from SDL event)
    tWindowContainer m_windows;  /// a list of application windows
};

#endif