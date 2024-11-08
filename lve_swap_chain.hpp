#pragma once

#include "lve_device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace lve {

class LveSwapChain {
public:
  // GPU와 CPU 간의 병렬 처리를 최적화
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  LveSwapChain(LveDevice &deviceRef, VkExtent2D windowExtent);
  /**
   * @brief 이전 스왑체인을 참조하여 새로운 스왑체인을 생성 -> 창 크기 변경 대비
   */
  LveSwapChain(LveDevice &deviceRef, VkExtent2D windowExtent,
               std::shared_ptr<LveSwapChain> previous);
  ~LveSwapChain();
  LveSwapChain(const LveSwapChain &) = delete;
  LveSwapChain operator=(const LveSwapChain &) = delete;

  // getter function
  VkFramebuffer getFrameBuffer(int index) {
    return swapChainFramebuffers[index];
  }
  VkRenderPass getRenderPass() { return renderPass; }
  VkImageView getImageView(int index) { return swapChainImageViews[index]; }
  size_t imageCount() { return swapChainImages.size(); }
  VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
  VkExtent2D getSwapChainExtent() { return swapChainExtent; }
  uint32_t width() { return swapChainExtent.width; }
  uint32_t height() { return swapChainExtent.height; }

  /**
   * @brief 스왑체인의 가로세로 비율을 반환
   *
   * @return width, height
   */
  float extentAspectRatio() {
    return static_cast<float>(swapChainExtent.width) /
           static_cast<float>(swapChainExtent.height);
  }

  /**
   * @brief  깊이 버퍼(Depth Buffer)에 적합한 포맷을 찾는 함수
   *
   * @return VkFormat
   */
  VkFormat findDepthFormat();

  /**
   * @brief GPU가 현재 프레임의 작업을 완료한 후, 다음 렌더링에 사용할 스왑체인
   * 이미지 가져오기
   *
   * @param imageIndex
   * @return VkResult
   */
  VkResult acquireNextImage(uint32_t *imageIndex);
  VkResult submitCommandBuffers(const VkCommandBuffer *buffers,
                                uint32_t *imageIndex);

  /**
   * @brief Swap chain format이 이전 Swap chain이랑 같은지 비교
   *
   * @param swapChain
   * @return true
   * @return false
   */
  bool compareSwapFormats(const LveSwapChain &swapChain) const {
    return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
           swapChain.swapChainImageFormat == swapChainImageFormat;
  }

private:
  void init();

  /**
   * @brief 스왑체인 생성에 필요한 설정을 구성 및 instace 생성
   *
   */
  void createSwapChain();

  /**
   * @brief  GPU가 해당 이미지를 렌더링 파이프라인에서 해석할 수 있도록 준비
   *
   */
  void createImageViews();

  /**
   * @brief attachments을 어떻게 초기화하고 저장할지 등의 설정 -> Render Pass
   *
   */
  void createRenderPass();

  /**
   * @brief 깊이 버퍼(depth buffer)를 생성하고 설정
   *  3D 렌더링에서 객체의 깊이(카메라로부터의 거리)를 저장
   *
   */
  void createDepthResources();

  /**
   * @brief  렌더 패스가 수행될 대상인 프레임버퍼를 생성
   *
   */
  void createFramebuffers();

  /**
   * @brief 세마포어(semaphore)와 펜스(fence) 같은 동기화 객체 생성
   *
   */
  void createSyncObjects();

  /**
   * @brief 스왑체인 표면 포맷을 선택
   * 화면에 표시될 이미지의 색상 포맷과 색상 공간을 정의
   *
   * @param availableFormats
   * @return VkSurfaceFormatKHR
   */
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  /**
   * @brief 스왑체인의 (present mode)를 선택
   *
   * @param availablePresentModes
   * @return VkPresentModeKHR
   */
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  /**
   * @brief 스왑체인의 이미지 크기(해상도)를 결정
   *
   * @param capabilities
   * @return VkExtent2D
   */
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat swapChainImageFormat;
  VkFormat swapChainDepthFormat;
  VkExtent2D swapChainExtent;

  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkRenderPass renderPass;

  std::vector<VkImage> depthImages;
  std::vector<VkDeviceMemory> depthImageMemorys;
  std::vector<VkImageView> depthImageViews;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;

  LveDevice &device;
  VkExtent2D windowExtent;

  VkSwapchainKHR swapChain;
  std::shared_ptr<LveSwapChain> oldSwapChain;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  std::vector<VkFence> imagesInFlight;
  size_t currentFrame = 0;
};

} // namespace lve
