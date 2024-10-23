#pragma once

#include <string>
#include <vector>

namespace lve {
class LvePipeline {
public:
  // vertex shader와 fragment shader 파일 경로를 받아서 pipeline 생성
  LvePipeline(const std::string &vertFilepath, const std::string &fragFilepath);

private:
  // 파일을 읽어서 char vector로 반환
  static std::vector<char> readFile(const std::string &filepath);

  // vertex shader와 fragment shader 파일 경로를 받아서 pipeline 생성
  void createGraphicsPipeline(const std::string &vertFilepath,
                              const std::string &fragFilepath);
};
} // namespace lve