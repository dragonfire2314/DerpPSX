#ifndef APP_hpp 
#define APP_hpp

#include "window.hh"
#include <renderer/OpenGL/opengl.hh>
#include <core.hh>
#include <gui.hh>
#include <runner/runner.hh>
#include <runner/debug/debugger.hh>
#include <configuration.hh>

#include <string>

class Application 
{
private:
    GLFWwindow* window;
    RenderAPI* renderer;

    Configuration configuration;

    GUI* debugger;
    Core core;
    Runner* runner;

    void imgui_init();
    void glfw_init();

    void loadConfiguration();

public:
    Application();
    void Run();

    std::string getBiosFile();

    friend class Core;
};

#endif