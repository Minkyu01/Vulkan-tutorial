#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

namespace lve {

class LveWindow {
public:
  LveWindow(int w, int h, std::string name);
  ~LveWindow();

  // 복사 생성자, 대입 연산자 삭제하여 리소스의 유일성을 보장
  LveWindow(const LveWindow &) = delete;
  LveWindow &operator=(const LveWindow &) = delete;

  // glfwWindowShouldClose 함수를 호출하여 창이 닫혔는지 확인
  bool shouldClose() { return glfwWindowShouldClose(window); }

  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
  void initWindow();
  const int width;
  const int height;
  std::string windowName;
  GLFWwindow *window;
};
} // namespace lve