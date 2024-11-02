//  # glsl버전 4.5에 사용
#version 450

// vertex shader
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

// fragment shader로 넘겨줄 변수 -> 각 shader마다 location은 독립적
// 그래서 location이 0으로 같아도 상관없음
layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  // 동차 좌표계
  mat4 transform;
  vec3 color;
}
push;

void main() {
  gl_Position = push.transform * vec4(position, 1.0);
  fragColor = color;
}