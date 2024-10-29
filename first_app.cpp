#include "first_app.hpp"
#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace lve {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
  SimpleRenderSystem simpleRenderSystem{lveDevice,
                                        lveRenderer.getSwapChainRenderPass()};

  while (!lveWindow.shouldClose()) {
    glfwPollEvents();

    // 스왑체인을 다시 생성해야 하는 경우 프레임 시작 함수가 null포인터를 반환!!
    // 스왑체인 시작 경로와 렌더 경로가 단일함수로 통합되어 있지 않는 이유
    // 애플리케이션단에서 아래 기능들을 제어하기를 원함 !
    // begin offscreen shadow pass
    // render shadow casting objects
    // end offscreen shadow pass
    if (auto commandBuffer = lveRenderer.beginFrame()) {
      lveRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
      lveRenderer.endSwapChainRenderPass(commandBuffer);
      lveRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadGameObjects() {
  std::vector<LveModel::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                         {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                         {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);

  auto triangle = LveGameObject::createGameObject();
  triangle.model = lveModel;
  triangle.color = {.1f, .8f, .1f};
  triangle.transform2d.translation.x = .2f;

  //   단위 벡터 크기 변환
  triangle.transform2d.scale = {2.f, .5f};
  // 라디언 사용, 2파이 = 360도, 0.25파이 * 2파이 = 90도
  triangle.transform2d.rotation = .25f * glm::two_pi<float>();

  gameObjects.push_back(std::move(triangle));
}

} // namespace lve