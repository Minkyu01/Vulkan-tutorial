#pragma once

#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace lve {

/**
 * @brief lve renderer instance 생성
 *
 * SwapChain 생성, Command Buffer 생성, Render Pass 생성, frame draw
 */
class LveRenderer {

public:
  LveRenderer(LveWindow &window, LveDevice &device);
  ~LveRenderer();

  LveRenderer(const LveRenderer &) = delete;
  LveRenderer &operator=(const LveRenderer &) = delete;

  //   draw 기능을 프레임 시작하는 함수와 프레임 끝내는 함수로 나눔
  /**
   * @brief 렌더링 프레임의 시작을 설정,
   * 스왑 체인이 재생성되어야 하는 경우 처리 -> 동적 할당으로 처리
   * 프레임을 위해 커맨드 버퍼를 초기화 하기
   * 스왑 체인에서 다음 이미지를 가져오기
   * 커맨드 버퍼 기록을 시작
   *
   * @return VkCommandBuffer
   */
  VkCommandBuffer beginFrame();

  /**
   * @brief 프레임의 렌더링을 마무리하는 역할
   * 프레임의 커맨드 버퍼 기록을 종료하고, 기록된 명령을 스왑 체인에 제출
   * 이상이 있으면 스왑 체인을 재 생성
   */
  void endFrame();

  /**
   * @brief  Vulkan에서 특정 프레임에 대한 렌더 패스(Render Pass) 시작
   * 초기화와 뷰포트 및 시저 설정을 통해 렌더링 환경을 구성
   *
   */
  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

  /**
   * @brief 랜더 패스에 commandBuffer 설정
   *
   */
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  /**
   * @brief pipeline 구성할때 render pass 반환
   *
   * @return renderPass
   */
  VkRenderPass getSwapChainRenderPass() const {
    return lveSwapChain->getRenderPass();
  }

  /**
   * @brief swapchain extent 반환
   *
   * @return width, height
   */
  float getAspectRatio() const { return lveSwapChain->extentAspectRatio(); }

  // getter function
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
  /**
   * @brief  렌더링 루프에서 각 프레임마다 사용할 커맨드 버퍼들을 미리 생성
   */
  void createCommandBuffers();

  /**
   * @brief 프로그램 종료시 커맨드 버퍼들을 해제
   *
   */
  void freeCommandBuffers();

  /**
   * @brief  창 크기 변경 등으로 인해 스왑체인을 재생성
   *
   */
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