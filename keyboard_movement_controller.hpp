#pragma once

#include "lve_game_object.hpp"
#include "lve_window.hpp"

namespace lve {

class KeyboardMovementController {
public:
  struct KeyMappings {
    int moveLeft = GLFW_KEY_A;
    int moveRight = GLFW_KEY_D;
    int moveForward = GLFW_KEY_W;
    int moveBackward = GLFW_KEY_S;
    int moveUp = GLFW_KEY_E;
    int moveDown = GLFW_KEY_Q;
    int lookLeft = GLFW_KEY_LEFT;
    int lookRight = GLFW_KEY_RIGHT;
    int lookUp = GLFW_KEY_UP;
    int lookDown = GLFW_KEY_DOWN;
  };

  /**
   * @brief 게임 오브젝트를 XZ 평면상에서 이동시킵니다.
   *
   * @param window
   * @param dt
   * @param gameObject
   */
  void moveInPlaneXZ(GLFWwindow *window, float dt, LveGameObject &gameObject);

  KeyMappings keys{};

  float moveSpeed{3.f};
  //   회전 속도
  float lookSpeed{1.5f};
};
} // namespace lve