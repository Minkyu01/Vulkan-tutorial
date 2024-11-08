#pragma once

#include "lve_device.hpp"

// std
#include <string>
#include <vector>

namespace lve {

struct PipelineConfigInfo {
  VkViewport viewport;
  VkRect2D scissor;
  VkPipelineViewportStateCreateInfo viewportInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  std::vector<VkDynamicState> dynamicStateEnables;
  VkPipelineDynamicStateCreateInfo dynamicStateInfo;
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class LvePipeline {
public:
  // vertex shader와 fragment shader 파일 경로를 받아서 pipeline 생성
  LvePipeline(LveDevice &device, const std::string &vertFilepath,
              const std::string &fragFilepath,
              const PipelineConfigInfo &configInfo);

  ~LvePipeline();

  LvePipeline(const LvePipeline &) = delete;
  LvePipeline operator=(const LvePipeline &) = delete;

  /**
   * @brief graphics pipeline에 Command Buffer를 바인딩
   *
   * @param commandBuffer
   */
  void bind(VkCommandBuffer commandBuffer);

  /**
   * @brief graphics pipeline 초기화
   *
   * @param configInfo
   */
  static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

private:
  /**
   * @brief 바이너리 모드로 읽어와 그 데이터를 벡터 형태로 반환
   *
   * @param filepath
   * @return std::vector<char>
   */
  static std::vector<char> readFile(const std::string &filepath);

  /**
   * @brief Graphics Pipeline 생성
   *    shader 모듈들 생성
   *
   * @param vertFilepath
   * @param fragFilepath
   * @param configInfo
   */
  void createGraphicsPipeline(const std::string &vertFilepath,
                              const std::string &fragFilepath,
                              const PipelineConfigInfo &configInfo);

  /**
   * @brief
   * 바이너리 데이터를 Shader Module로 변환
   *
   * @param code
   * @param shaderModule
   */
  void createShaderModule(const std::vector<char> &code,
                          VkShaderModule *shaderModule);

  LveDevice &lveDevice;
  VkPipeline graphicsPipeline;
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
};
} // namespace lve