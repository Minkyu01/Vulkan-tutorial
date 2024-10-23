#pragma once

#include "lve_pipeline.hpp"
#include "lve_window.hpp"

namespace lve {

class FirstApp {

public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  void run();

private:
  // window객체 생성
  LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};

  // vertex, fragment shader 파일 경로를 받아서 pipeline 생성
  LvePipeline lvePipeline{"shaders/simple_shader.vert.spv",
                          "shaders/simple_shader.frag.spv"};
};
} // namespace lve