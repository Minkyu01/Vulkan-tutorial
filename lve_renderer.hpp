#pragma once

#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace lve {

class LveRenderer {

public:
  LveRenderer(LveWindow &window, LveDevice &device);
  ~LveRenderer();

  LveRenderer(const LveRenderer &) = delete;
  LveRenderer &operator=(const LveRenderer &) = delete;

  //   draw 기능을 프레임 시작하는 함수와 프레임 끝내는 함수로 나눔
  VkCommandBuffer beginFrame();
  void endFrame();

  //  render pass에 swap chain 그리기 시작 및 끝내는 함수
  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  // 파이프라인을 구성하기 위해 swap chain render pass에 접근할수 있어야함
  VkRenderPass getSwapChainRenderPass() const {
    return lveSwapChain->getRenderPass();
  }

  float getAspectRatio() const { return lveSwapChain->extentAspectRatio(); }

  //  프레임 진행중인지 확인하는 함수
  bool isFrameInProgress() { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted &&
           "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getFrameIndex() const {
    assert(isFrameStarted &&
           "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
  }

private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapChain();

  LveWindow &lveWindow;
  LveDevice &lveDevice;
  std::unique_ptr<LveSwapChain> lveSwapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  // 현재 진행중인 프레임 상태 추적
  uint32_t currentImageIndex;

  //   frame index를 추적, 이미지 인덱스에 연결되지 않은 프레임중
  int currentFrameIndex;
  bool isFrameStarted;
};
} // namespace lve