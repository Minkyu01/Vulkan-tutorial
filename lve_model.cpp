#include "lve_model.hpp"

// std
#include <cassert>
#include <cstring>

namespace lve {

LveModel::LveModel(LveDevice &device, const std::vector<Vertex> &vertices)
    : lveDevice{device} {
  createVertexBuffers(vertices);
}

LveModel::~LveModel() {
  vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
  vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
}

void LveModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
  // 정점이 3개 이상만 vertex모듈로 인실
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

  // 버퍼 생성
  lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         vertexBuffer, vertexBufferMemory);

  void *data;
  //  장치 메모리에 매핑된 호스트 메모리 생성, 매핑된 메모리 가져오기
  vkMapMemory(lveDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(lveDevice.device(), vertexBufferMemory);
}

void LveModel::draw(VkCommandBuffer commandBuffer) {
  vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

void LveModel::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};

  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

std::vector<VkVertexInputBindingDescription>
LveModel::Vertex::getBindingDescriptions() {
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription>
LveModel::Vertex::getAttributeDescriptions() {
  // 2개의 attribute를 가진 정점 구조체
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, position);

  // interleaved 하게 binding하기 때문에 0
  attributeDescriptions[1].binding = 0;
  // vertex shader에서 사용하는 location과 같아야 함 : 1
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  // 색상 데이터 offset 자동 계산
  attributeDescriptions[1].offset = offsetof(Vertex, color);
  return attributeDescriptions;
}

} // namespace lve