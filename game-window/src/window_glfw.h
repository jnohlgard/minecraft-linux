#pragma once

#include <game_window.h>
#include <GLFW/glfw3.h>

class GLFWGameWindow : public GameWindow {

private:
    GLFWwindow* window;
    double lastMouseX = 0.0, lastMouseY = 0.0;
    int windowedX = -1, windowedY = -1;
    int windowedWidth = -1, windowedHeight = -1;
    int relativeScale;
    bool resized = false;
    bool focused = true;
    bool warnedButtons = false;

    friend class GLFWJoystickManager;

    static KeyCode getKeyMinecraft(int keyCode);

    static void _glfwWindowSizeCallback(GLFWwindow* window, int w, int h);
    static void _glfwCursorPosCallback(GLFWwindow* window, double x, double y);
    static void _glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void _glfwScrollCallback(GLFWwindow* window, double x, double y);
    static void _glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _glfwCharCallback(GLFWwindow* window, unsigned int ch);
    static void _glfwWindowCloseCallback(GLFWwindow* window);
    static void _glfwWindowFocusCallback(GLFWwindow* window, int focused);
    static void _glfwWindowContentScaleCallback(GLFWwindow* window, float scalex, float scaley);

public:

    GLFWGameWindow(const std::string& title, int width, int height, GraphicsApi api);

    ~GLFWGameWindow() override;

    void setIcon(std::string const& iconPath) override;

    void makeCurrent(bool active) override;

    int getRelativeScale() const;

    void setRelativeScale();

    void show() override;

    void close() override;

    void pollEvents() override;

    void setCursorDisabled(bool disabled) override;

    void setFullscreen(bool fullscreen) override;

    void getWindowSize(int& width, int& height) const override;

    void setClipboardText(std::string const& text) override;

    void swapBuffers() override;

    void setSwapInterval(int interval) override;

};
