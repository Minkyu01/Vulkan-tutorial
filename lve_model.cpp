#include "lve_model.hpp"
#include "lve_utils.hpp"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
// hash function
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace std {
template <> struct hash<lve::LveModel::Vertex> {
  size_t operator()(lve::LveModel::Vertex const &vertex) const {
    size_t seed = 0;
    lve::hashCombine(seed, vertex.position, vertex.color, vertex.normal,
                     vertex.uv);
    return seed;
  }
};
} // namespace std

namespace lve {

LveModel::LveModel(LveDevice &device, const LveModel::Builder &builder)
    : lveDevice{device} {
  createVertexBuffers(builder.vertices);
  createIndexBuffers(builder.indices);
}

LveModel::~LveModel() {
  vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
  vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);

  if (hasIndexBuffer) {
    vkDestroyBuffer(lveDevice.device(), indexBuffer, nullptr);
    vkFreeMemory(lveDevice.device(), indexBufferMemory, nullptr);
  }
}

std::unique_ptr<LveModel>
LveModel::createModelFromFile(LveDevice &device, const std::string &filepath) {
  Builder builder{};
  builder.loadModel(filepath);

  // index buffer로 계산할때와 vertex buffer로 계산할때 차이
  std::cout << "Vertex count " << builder.vertices.size() << std::endl;

  return std::make_unique<LveModel>(device, builder);
}

void LveModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
  // 정점이 3개 이상만 vertex모듈로 인실
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  // 버퍼 생성 -> VK_BUFFER_USAGE_TRANSFER_SRC_BIT
  // 해당 옵션이 vulkan에게 이 버퍼가 다른 버퍼로 복사될 것이라고 알려줌
  // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT -> staging buffer는 host에서 볼수있음
  // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT-> host와device간의 메모리 일관성을유지
  lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory);

  void *data;
  //  장치 메모리에 매핑된 호스트 메모리 생성, 매핑된 메모리 가져오기
  vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(lveDevice.device(), stagingBufferMemory);

  lveDevice.createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

  lveDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

  vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
  vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);
}

void LveModel::createIndexBuffers(const std::vector<uint32_t> &indices) {
  indexCount = static_cast<uint32_t>(indices.size());

  //  인덱스가 없으면 종료 => 정점이 없다
  hasIndexBuffer = indexCount > 0;
  if (!hasIndexBuffer) {
    return;
  }

  VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory);
  void *data;

  vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(lveDevice.device(), stagingBufferMemory);

  lveDevice.createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

  lveDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

  vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
  vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);
}

void LveModel::draw(VkCommandBuffer commandBuffer) {
  //   vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  if (hasIndexBuffer) {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  } else {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  }
}

void LveModel::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};

  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
  }
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

void LveModel::Builder::loadModel(const std::string &filepath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        filepath.c_str())) {
    throw std::runtime_error(warn + err);
  }

  vertices.clear();
  indices.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};

      if (index.vertex_index >= 0) {
        vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2],
        };

        auto colorIndex = 3 * index.vertex_index + 2;
        if (colorIndex < attrib.colors.size()) {
          vertex.color = {
              attrib.colors[colorIndex - 2],
              attrib.colors[colorIndex - 1],
              attrib.colors[colorIndex - 0],
          };
        } else {
          // 색상 지정 안되었을때 기본 값 설정
          vertex.color = {1.f, 1.f, 1.f};
        }
      }

      if (index.normal_index >= 0) {
        vertex.normal = {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2],
        };
      }

      if (index.texcoord_index >= 0) {
        vertex.uv = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            attrib.texcoords[2 * index.texcoord_index + 1],
        };
      }

      //   vertices.push_back(vertex);

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

} // namespace lve