#include "gamenet_app/app/glfw_vulkan_app.h"

#include <GLFW/glfw3.h>

#include "core/logger/logger.h"
#include "glfw_vulkan_app.h"

// TODO: DON'T hard-code screen width.
// Move it to a configuration file.
static const int kWidth = 1280;
static const int kHeight = 720;

int GameNet::GLFWApp::Init(int argc, char** argv) {
  if (!glfwInit()) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: glfwInit() error\n", __FUNCTION__);
    return 1;
  }

  if (!glfwVulkanSupported()) {
    glfwTerminate();
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: Vulkan is not supported\n",
                __FUNCTION__);
    return 1;
  }

  /* Vulkan needs no context */
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  /* Window creation */
  _window =
      glfwCreateWindow(kWidth, kHeight, _windowTitle.c_str(), nullptr, nullptr);
  if (!_window) {
    glfwTerminate();
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: Could not create window\n",
                __FUNCTION__);
    return 1;
  }

  /* Set event callbacks */
  glfwSetWindowUserPointer(_window, this);
  glfwSetWindowSizeCallback(_window, &ResizeCallback);
  glfwSetKeyCallback(_window, &KeyEventCallback);
  glfwSetMouseButtonCallback(_window, &MouseButtonCallback);
  glfwSetCursorPosCallback(_window, &MousePositionCallback);

  return OnInit(argc, argv);
}

int GameNet::GLFWApp::Run() { 
  int res = 0;
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
		res = OnTick(); 
    if (res != 0) {
      RequestWindowClose();
    }
    glfwSwapBuffers(_window);
	}
  return res;
}

void GameNet::GLFWApp::Shutdown() {
  OnShutdown();

  glfwDestroyWindow(_window);
  glfwTerminate();
}

void GameNet::GLFWApp::SetWindowTitle(const std::string& windowTitle) {
  _windowTitle = windowTitle;
  glfwSetWindowTitle(_window, _windowTitle.c_str());
}

const std::string& GameNet::GLFWApp::GetWindowTitle() const {
  return _windowTitle;
}

void GameNet::GLFWApp::RequestWindowClose() {
  glfwSetWindowShouldClose(_window, GLFW_TRUE);
}

void GameNet::GLFWApp::ResizeCallback(GLFWwindow* win, int width, int height) {
  auto app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(win));
  app->OnResize(width, height);
}

void GameNet::GLFWApp::KeyEventCallback(GLFWwindow* win, int key, int scancode,
                                        int action, int mods) {
  auto app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(win));
  app->OnKey(key, scancode, action, mods);
}

void GameNet::GLFWApp::MouseButtonCallback(GLFWwindow* win, int button,
                                           int action, int mods) {
  auto app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(win));
  app->OnMouseButton(button, action, mods);
}

void GameNet::GLFWApp::MousePositionCallback(GLFWwindow* win, double xpos,
                                             double ypos) {
  auto app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(win));
  app->OnMouseMove(xpos, ypos);
}
