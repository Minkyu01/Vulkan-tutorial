#pragma once

#include "lve_device.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace lve {

class LveModel {
public:
  struct Vertex {
    glm::vec3 position{};
    glm::vec3 color{};
    // vn
    glm::vec3 normal{};
    // vt
    glm::vec2 uv{};

    // binding description
    static std::vector<VkVertexInputBindingDescription>
    getBindingDescriptions();

    // attribute descriptions
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions();

    bool operator==(const Vertex &other) const {
      return position == other.position && color == other.color &&
             normal == other.normal && uv == other.uv;
    }
  };

  //   임시 저장소 -> index buffer 메모리에 저장될때 까지 사용
  //   vertex buffer만 있거나 index buffer가 같이 있는 모델 둘다 사용 가능
  struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    void loadModel(const std::string &filepath);
  };

  LveModel(LveDevice &device, const LveModel::Builder &builder);
  ~LveModel();

  LveModel(const LveModel &) = delete;
  LveModel &operator=(const LveModel &) = delete;

  static std::unique_ptr<LveModel>
  createModelFromFile(LveDevice &device, const std::string &filepath);

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

private:
  void createVertexBuffers(const std::vector<Vertex> &vertices);
  void createIndexBuffers(const std::vector<uint32_t> &indices);

  LveDevice &lveDevice;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
  uint32_t indexCount;
};

} // namespace lve