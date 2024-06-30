#include "Window.h"

#include "Engine.h"
#include "Renderer.h"

#include <iostream>
#include <stdexcept>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

Window::Window(Renderer::RendererApi const renderer_api, i32 const screen_width, i32 const screen_height, i32 const subsamples,
               bool const enable_vsync, bool const enable_mouse_capture)
{
    switch (renderer_api)
    {
    case Renderer::RendererApi::DirectX11:
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_SAMPLES, GLFW_DONT_CARE);
    }
    break;
    default:
    {
        std::unreachable();
    }
    }

    m_window = glfwCreateWindow(screen_width, screen_height, "Engine", nullptr, nullptr);

    if (m_window == nullptr)
        throw std::runtime_error("Could not create a window.");

    switch (renderer_api)
    {
    case Renderer::RendererApi::DirectX11:
    {
        if (enable_mouse_capture)
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        break;
    }
    default:
    {
        std::unreachable();
    }
    }
}

GLFWwindow* Window::get_glfw_window() const
{
    return m_window;
}

HWND Window::get_win32_window() const
{
    return glfwGetWin32Window(m_window);
}

void Window::maximize_glfw_window() const
{
    glfwMaximizeWindow(m_window);
}
