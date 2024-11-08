#pragma once

#include "lve_camera.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace lve {

class SimpleRenderSystem {

public:
  /**
   * @brief pipeline layout과 pipeline 생성
   *
   */
  SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(VkCommandBuffer commandBuffer,
                         std::vector<LveGameObject> &gameObjects,
                         const LveCamera &camera);

private:
  /**
   * @brief 그래픽스 파이프라인 레이아웃을 생성
   * 푸시 상수를 설정, shader의 상수 데이터 설정
   *
   */
  void createPipelineLayout();

  /**
   * @brief 그래픽스 파이프라인 레이아웃을 이용해 그래픽 파이프라인 생성
   * shader 파일 정보로 파이프라인 객체 생성
   * @param renderPass
   */
  void createPipeline(VkRenderPass renderPass);

  // vertex, fragment shader 파일 경로를 받아서 pipeline 생성
  LveDevice &lveDevice;

  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace lve