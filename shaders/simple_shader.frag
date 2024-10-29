// # glsl버전 4.5에 사용
#version 450

// layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
  // glm 내장 변수 2x2 행렬
  mat2 transform;
  vec2 offset;
  vec3 color;
}
push;

// vertex shader과 다르게 내장된 출력 변수가 없으므로 직접 main 선언
void main() { outColor = vec4(push.color, 1.0); }