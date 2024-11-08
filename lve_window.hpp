#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

namespace lve {

/**
 * @brief window instance 생성
 */
class LveWindow {
public:
  /**
   * @brief 초기 window 생성
   *
   * @param w width
   * @param h height
   * @param name window name
   */
  LveWindow(int w, int h, std::string name);
  ~LveWindow();

  // 복사 생성자, 대입 연산자 삭제하여 리소스의 유일성을 보장
  LveWindow(const LveWindow &) = delete;
  LveWindow &operator=(const LveWindow &) = delete;

  /**
   * @brief  glfwWindowShouldClose 반환값으로 창이 닫혔는지 확인
   *
   * @return boolean
   */
  bool shouldClose() { return glfwWindowShouldClose(window); }

  /**
   * @brief 현재 window 크기 반환
   *
   * @return VkExtent2D
   */
  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }

  /**
   * @brief window 크기 조정 여부 확인
   *
   * @return boolean
   */
  bool wasWindowResized() { return framebufferResized; }

  /**
   * @brief window 크기 조정 flag 초기화
   */
  void resetWindowResizedFlag() { framebufferResized = false; }

  /**
   * @brief glfw window pointer 반환
   *
   * @return GLFWwindow*
   */
  GLFWwindow *getGLFWwindow() const { return window; }

  /**
   * @brief window surface 생성
   *
   * @param instance vulkan instance
   * @param surface vulkan surface
   */
  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
  /**
   * @brief 창 크기가 조절될때 호출되는 함수
   *
   * @param window glfw window
   * @param width resize width
   * @param height resize height
   */
  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height);

  /**
   * @brief glfw window 생성
   */
  void initWindow();

  int width;
  int height;
  bool framebufferResized = false;

  std::string windowName;
  GLFWwindow *window;
};
} // namespace lve