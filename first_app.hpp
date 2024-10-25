#pragma once

#include "lve_device.hpp"
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
  LveDevice lveDevice{lveWindow};
  LvePipeline lvePipeline{
      lveDevice, "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv",
      LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
};
} // namespace lve