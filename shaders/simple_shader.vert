//  # glsl버전 4.5에 사용
#version 450

// vertex shader
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

// fragment shader로 넘겨줄 변수 -> 각 shader마다 location은 독립적
// 그래서 location이 0으로 같아도 상관없음
layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  mat2 transform;
  vec2 offset;
  vec3 color;
}
push;

void main() {
  //  행렬 곱셈이므로 교한법칙이 성립하지 않음
  gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0);
}