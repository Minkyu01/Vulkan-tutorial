#pragma once

#include "lve_model.hpp"
// std
#include <memory>

// 각 게임 object들의 기본 클래스
namespace lve {

// 객체 움직일때 사용하는 구조체
struct Transform2dComponent {
  // (position offset)
  glm::vec2 translation{};

  //  항등행렬로 초기화
  glm::vec2 scale{1.f, 1.f};
  float rotation;

  glm::mat2 mat2() {
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);
    glm::mat2 rotMatrix{{c, s}, {-s, c}};
    glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
    return rotMatrix * scaleMat;
  }
};

class LveGameObject {
public:
  using id_t = unsigned int;

  static LveGameObject createGameObject() {
    static id_t currentId = 0;
    // 증감연산자 잊지 말기
    return LveGameObject(currentId++);
  }

  LveGameObject(const LveGameObject &) = delete;
  LveGameObject &operator=(const LveGameObject &) = delete;

  //   이건 뭐지?
  LveGameObject(LveGameObject &&) = default;
  LveGameObject &operator=(LveGameObject &&) = default;

  id_t getId() { return id; }

  std::shared_ptr<LveModel> model{};
  glm::vec3 color{};
  Transform2dComponent transform2d{};

private:
  LveGameObject(id_t objId) : id(objId) {};

  id_t id;
};

} // namespace lve