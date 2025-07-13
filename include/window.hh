#ifndef WINDOW_hpp 
#define WINDOW_hpp

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>

class Window 
{
private:
    int width;
    int height;
    std::string windowName;

    GLFWwindow* window;
public:
    Window(int _width, int _height, std::string _windowName);

    void imgui_init();
    void glfw_init();

    GLFWwindow* getGLFWwindow();
};

#endif