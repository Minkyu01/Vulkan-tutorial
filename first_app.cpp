#include "first_app.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace lve {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
  SimpleRenderSystem simpleRenderSystem{lveDevice,
                                        lveRenderer.getSwapChainRenderPass()};
  LveCamera camera{};

  // camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
  // 2.5f는 큐브의 중심
  camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

  // 렌더링은 되지 않고 카메라 현재 상태를 저장
  auto viewerObject = LveGameObject::createGameObject();
  KeyboardMovementController cameraController{};

  // 고성능 시간 측정
  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!lveWindow.shouldClose()) {
    glfwPollEvents();

    //  각 루프마다 시간 측정
    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count();
    currentTime = newTime;

    // 카메라 이동
    cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime,
                                   viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    float aspect = lveRenderer.getAspectRatio();
    // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

    if (auto commandBuffer = lveRenderer.beginFrame()) {
      lveRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
      lveRenderer.endSwapChainRenderPass(commandBuffer);
      lveRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadGameObjects() {
  std::shared_ptr<LveModel> lveModel =
      LveModel::createModelFromFile(lveDevice, "models/42/teapot.obj");

  auto gameObj = LveGameObject::createGameObject();

  gameObj.model = lveModel;
  gameObj.transform.translation = {.0f, .0f, 2.5f};
  gameObj.transform.scale = {.5f, .5f, .5f};
  //   gameObj.transform.scale = glm::vec3(3.f);

  gameObjects.push_back(std::move(gameObj));
}

} // namespace lve