#pragma once

#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"
#include "lve_swap_chain.hpp"
#include "lve_window.hpp"

// std
#include <memory>
#include <vector>

namespace lve {

class FirstApp {

public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();

private:
  void loadGameObjects();
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();
  void freeCommandBuffers();
  void recreateSwapChain();
  void recordCommandBuffer(int imageIndex);
  void renderGameObjects(VkCommandBuffer commandBuffer);

  // window객체 생성
  LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};

  // vertex, fragment shader 파일 경로를 받아서 pipeline 생성
  LveDevice lveDevice{lveWindow};

  //   LveSwapChain lveSwapChain{lveDevice, lveWindow.getExtent()};
  // 성능을 위해 unique_ptr로 변경
  std::unique_ptr<LveSwapChain> lveSwapChain;
  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::vector<LveGameObject> gameObjects;
};
} // namespace lve