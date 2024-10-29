#include "first_app.hpp"

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

struct SimplePushConstantData {
  // 단위 행렬로 초기화 -> 원행렬 * 단위행려 = 원행렬
  glm::mat2 transform{1.f};
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

FirstApp::FirstApp() {
  loadGameObjects();
  createPipelineLayout();
  //   createPipeline();
  recreateSwapChain();
  createCommandBuffers();
}
FirstApp::~FirstApp() {
  vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
}

void FirstApp::run() {
  while (!lveWindow.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadGameObjects() {
  std::vector<LveModel::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                         {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                         {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);

  std::vector<glm::vec3> colors{
      {1.f, .7f, .73f},
      {1.f, .87f, .73f},
      {1.f, 1.f, .73f},
      {.73f, 1.f, .8f},
      {.73, .88f, 1.f} //
  };
  for (auto &color : colors) {
    color = glm::pow(color, glm::vec3{2.2f});
  }
  for (int i = 0; i < 40; i++) {
    auto triangle = LveGameObject::createGameObject();
    triangle.model = lveModel;
    triangle.transform2d.scale = glm::vec2(.5f) + i * 0.025f;
    triangle.transform2d.rotation = i * glm::pi<float>() * .025f;
    triangle.color = colors[i % colors.size()];
    gameObjects.push_back(std::move(triangle));
  }

  // 원본 학습 결과
  //   auto triangle = LveGameObject::createGameObject();
  //   triangle.model = lveModel;
  //   triangle.color = {.1f, .8f, .1f};
  //   triangle.transform2d.translation.x = .2f;

  //   //   단위 벡터 크기 변환
  //   triangle.transform2d.scale = {2.f, .5f};
  //   // 라디언 사용, 2파이 = 360도, 0.25파이 * 2파이 = 90도
  //   triangle.transform2d.rotation = .25f * glm::two_pi<float>();

  //   gameObjects.push_back(std::move(triangle));
}

void FirstApp::recreateSwapChain() {
  auto extent = lveWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = lveWindow.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(lveDevice.device());
  if (lveSwapChain == nullptr) {
    lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
  } else {
    lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent,
                                                  std::move(lveSwapChain));
    if (lveSwapChain->imageCount() != commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }
  createPipeline();
}

void FirstApp::createPipelineLayout() {
  // push constant 설정
  VkPushConstantRange pushConstantRange{};
  //   vertex shader와 fragment shader에서 사용할 수 있도록 설정
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void FirstApp::createPipeline() {
  assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");
  PipelineConfigInfo pipelineConfig{};

  LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = lveSwapChain->getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  lvePipeline = std::make_unique<LvePipeline>(
      lveDevice, "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv", pipelineConfig);
}

void FirstApp::createCommandBuffers() {
  commandBuffers.resize(lveSwapChain->imageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = lveDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void FirstApp::freeCommandBuffers() {
  vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

void FirstApp::recordCommandBuffer(int imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = lveSwapChain->getRenderPass();
  renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  //   clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
  viewport.height =
      static_cast<float>(lveSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, lveSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

  // 게임 오브젝트 렌더링
  renderGameObjects(commandBuffers[imageIndex]);

  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {
  // update
  int i = 0;
  for (auto &obj : gameObjects) {
    i += 1;
    obj.transform2d.rotation = glm::mod<float>(
        obj.transform2d.rotation + 0.001f * i, 2.f * glm::pi<float>());
  }

  // render
  lvePipeline->bind(commandBuffer);
  for (auto &obj : gameObjects) {
    SimplePushConstantData push{};
    push.offset = obj.transform2d.translation;
    push.color = obj.color;
    push.transform = obj.transform2d.mat2();

    vkCmdPushConstants(commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
    obj.model->bind(commandBuffer);
    obj.model->draw(commandBuffer);
  }
}

// 원본 학습 결과
// void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {
//   lvePipeline->bind(commandBuffer);

//   for (auto &obj : gameObjects) {
//     // 삼각형을 완전한 원으로 계속 회전시키기
//     obj.transform2d.rotation =
//         glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

//     SimplePushConstantData push{};
//     push.offset = obj.transform2d.translation;
//     push.color = obj.color;
//     push.transform = obj.transform2d.mat2();

//     vkCmdPushConstants(commandBuffer, pipelineLayout,
//                        VK_SHADER_STAGE_VERTEX_BIT |
//                            VK_SHADER_STAGE_FRAGMENT_BIT,
//                        0, sizeof(SimplePushConstantData), &push);

//     obj.model->bind(commandBuffer);
//     obj.model->draw(commandBuffer);
//   }
// }

void FirstApp::drawFrame() {
  uint32_t imageIndex;
  auto result = lveSwapChain->acquireNextImage(&imageIndex);

  // 창 크기 변경, 사이즈 변경할때 호출 -> swapchain 재생성
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  //   result =
  //   lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex],
  //                                               &imageIndex);

  recordCommandBuffer(imageIndex);
  result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex],
                                              &imageIndex);

  //  swap chain의 상태를 감시하고, 교체(recreate) 작업이 필요한 경우
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      lveWindow.wasWindowResized()) {
    lveWindow.resetWindowResizedFlag();
    recreateSwapChain();
    return;
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }
}

} // namespace lve