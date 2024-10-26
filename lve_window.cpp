#include "lve_window.hpp"

// std
#include <stdexcept>

namespace lve {

LveWindow::LveWindow(int w, int h, std::string name)
    : width{w}, height{h}, windowName{name} {
  initWindow();
}

// 소멸자 순서 중요!!
LveWindow::~LveWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void LveWindow::initWindow() {
  glfwInit();

  //   OpenGL 대신 Vulkan이므로 OpenGL 컨텍스트를 생성하지 않도록 한다
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  //    창이 크기 조정을 허용하지 않도록 설정한다
  //   glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window =
      glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

  // glfw pointer 가져와서 callback 함수에 전달
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void LveWindow::createWindowSurface(VkInstance instance,
                                    VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void LveWindow::framebufferResizeCallback(GLFWwindow *window, int width,
                                          int height) {
  auto lveWindow =
      reinterpret_cast<LveWindow *>(glfwGetWindowUserPointer(window));
  lveWindow->framebufferResized = true;
  lveWindow->width = width;
  lveWindow->height = height;
}

} // namespace lve