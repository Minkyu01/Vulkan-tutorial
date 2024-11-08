#pragma once

#include "lve_window.hpp"

// std lib headers
#include <string>
#include <vector>

namespace lve {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  uint32_t graphicsFamily;
  uint32_t presentFamily;
  bool graphicsFamilyHasValue = false;
  bool presentFamilyHasValue = false;
  bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

/**
 * @brief device instance 생성
 *
 * Vulkan Instance 설정, 디버그 메세지 설정 , glfw와 surface 연결
 * Physical Device 선택, Logical Device 설정, CommandPool 생성
 */
class LveDevice {
public:
/**
 * @brief 배포할 때에는 디버그 레이어를 끄고, 개발할 때에는 켠다
 *
 */
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

  LveDevice(LveWindow &window);
  ~LveDevice();

  // Not copyable or movable
  LveDevice(const LveDevice &) = delete;
  LveDevice operator=(const LveDevice &) = delete;
  LveDevice(LveDevice &&) = delete;
  LveDevice &operator=(LveDevice &&) = delete;

  // getter function
  VkCommandPool getCommandPool() { return commandPool; }
  VkDevice device() { return device_; }
  VkSurfaceKHR surface() { return surface_; }
  VkQueue graphicsQueue() { return graphicsQueue_; }
  VkQueue presentQueue() { return presentQueue_; }

  SwapChainSupportDetails getSwapChainSupport() {
    return querySwapChainSupport(physicalDevice);
  }
  QueueFamilyIndices findPhysicalQueueFamilies() {
    return findQueueFamilies(physicalDevice);
  }

  /**
   * @brief  Vulkan에서 메모리 할당을 위한 메모리 유형을 선택
   *
   * @param typeFilter 특정 메모리 유형 비트를 나타내는 필터
   * @param properties 필요한 메모리 속성 플래그(VkMemoryPropertyFlags)
   * @return uint32_t
   */
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

  /**
   * @brief  Vulkan 애플리케이션에서 특정 텍스처나 이미지에 필요한
   * 형식(format)을 찾기 위해 사용
   *
   * @param candidates 사용할 수 있는 형식 목록
   * @param tiling 이미지 타일링 방식 설정
   * @param features 요구되는 형식 기능 플래그
   * @return VkFormat
   */
  VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);

  /**
   * @brief  Vulkan에서 버퍼를 생성하고 필요한 메모리를 할당
   *
   * @param size 버퍼의 크기
   * @param usage 버퍼의 사용 용도
   * @param properties 할당할 메모리의 속성 지정
   * @param buffer 버퍼 객체
   * @param bufferMemory 버퍼에 할당된 메모리를 가리킴
   */
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);

  /**
   * @brief  간단한 작업을 위해 일회성 커맨드 버퍼를 생성하고 시작
   */
  VkCommandBuffer beginSingleTimeCommands();

  /**
   * @brief 한 일회성 커맨드 버퍼를 완료하고 command pool에 제출
   *
   * @param commandBuffer
   */
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);

  /**
   * @brief  한 버퍼의 데이터를 다른 버퍼로 복사
   *
   * @param srcBuffer 원본 버퍼
   * @param dstBuffer 대상 버퍼
   * @param size 버퍼 크기
   */
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  /**
   * @brief   텍스처 이미지 데이터를 CPU에서 준비하여 GPU로 복사할 때 사용
   * 이미지를 CPU에서 로드 ->  그 데이터를 Vulkan 버퍼에 저장
   * -> 이 데이터를 GPU에서 텍스처로 사용 할수 있도록 이미지 리소스로 복사
   *
   * @param buffer 원본 버퍼
   * @param image 복사 대상 이미지
   * @param width
   * @param height
   * @param layerCount 복사할 이미지의 레이어 수
   */
  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height, uint32_t layerCount);

  /**
   * @brief 이미지 리소스를 생성할 때, 메모리 할당과 바인딩을 명시
   *
   * @param imageInfo 이미지 생성 정보
   * @param properties 할당할 메모리의 속성
   * @param image 생성된 이미지 객체
   * @param imageMemory 이미지에 할당된 메모리
   */
  void createImageWithInfo(const VkImageCreateInfo &imageInfo,
                           VkMemoryPropertyFlags properties, VkImage &image,
                           VkDeviceMemory &imageMemory);

  /**
   * @brief 물리적 장치의 특성 정보를 저장
   */
  VkPhysicalDeviceProperties properties;

private:
  /**
   * @brief 유효성 검사 레이어 설정, 애플리케이션과 vulkan 연결 instance 생성
   *
   */
  void createInstance();

  /**
   * @brief  Debug 메시지 설정
   *
   */
  void setupDebugMessenger();

  /**
   * @brief glfw에서 vulkan을 사용할 수 있도록 surface(window) 생성
   *
   */
  void createSurface();

  //
  /**
   * @brief 애플리케이션이 사용하는 물리적 장치 선택
   * ex: 3060ti, 3080, 3090, 등등, 복수 선택도 가능은 함
   *
   */
  void pickPhysicalDevice();

  /**
   * @brief  우리가 사용하려는 물리적 장치의 기능을 설명
   *
   */
  void createLogicalDevice();

  /**
   * @brief 나중에 사용하는 명령어 pool을 만들어 놓기
   *
   */
  void createCommandPool();

  /**
   * @brief  Vulkan이 사용할 GPU가 특정 요구 사항을 충족하는지 검사
   *
   * @param device
   * @return boolean
   */
  bool isDeviceSuitable(VkPhysicalDevice device);

  /**
   * @brief  Vulkan 애플리케이션이 실행되기 위해 필요한 확장목록을 반환
   * -> 필요한 확장이 있으면 추가해도 됨
   *
   * @return std::vector<const char *>
   */
  std::vector<const char *> getRequiredExtensions();

  /**
   * @brief  주어진 물리적 장치가 필요한 확장을 지원하는지 확인
   *
   * @param device
   * @return boolean
   */
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  /**
   * @brief  요청된 유효성 검사 레이어들이 운영체제에서 지원되는지 확인
   *
   * @return boolean
   */
  bool checkValidationLayerSupport();

  /**
   * @brief  필요한 인스턴스 확장 기능을 운영체제에서 지원하는지 확인하는 함수
   *
   */
  void hasGflwRequiredInstanceExtensions();

  /**
   * @brief  Vulkan 애플리케이션이 선택된 물리적 디바이스에서  사용할 수 있는 큐
   * 패밀리(queue family)를 찾음
   *
   * @param device
   * @return QueueFamilyIndices
   */
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  /**
   * @brief 디버그 메시지 설정을 위한 디버깅 구조체 초기화
   *
   * @param createInfo
   */
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  /**
   * @brief 물리적 장치의 swap chain 지원 정보를 반환
   *
   * @param device
   * @return SwapChainSupportDetails
   */
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  // Vulkan 객체들 (인스턴스, 디바이스, 명령 풀 등)
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  LveWindow &window;
  VkCommandPool commandPool;

  VkDevice device_;
  VkSurfaceKHR surface_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;

  // validation layer 설정
  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      "VK_KHR_portability_subset",
  };
};

} // namespace lve