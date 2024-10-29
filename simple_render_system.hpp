#pragma once

#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace lve {

class SimpleRenderSystem {

public:
  SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(VkCommandBuffer commandBuffer,
                         std::vector<LveGameObject> &gameObjects);

private:
  void createPipelineLayout();
  // 전달된 render passs는 수명주기가 렌더 패스에 연결되어 있지 않기 때문에 렌더
  // 패스 저장 안함
  void createPipeline(VkRenderPass renderPass);

  // vertex, fragment shader 파일 경로를 받아서 pipeline 생성
  LveDevice &lveDevice;

  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace lve