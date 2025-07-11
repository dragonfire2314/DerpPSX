#ifndef APP_hpp 
#define APP_hpp

#include "window.h"
#include <renderer/OpenGL/opengl.h>
#include <core.h>
#include <gui.h>
#include <runner/runner.hh>
#include <runner/debug/debugger.h>

#include <string>

struct Application_Config 
{
    int width;
    int height;
    std::string windowName;
};

class Application 
{
private:
    Application_Config config;
    GLFWwindow* window;
    RenderAPI* renderer;

    GUI* debugger;
    Core core;
    Runner* runner;

    void imgui_init();
    void glfw_init();

public:
    Application(Application_Config &_config);
    void Run();

    friend class Core;
};

#endif