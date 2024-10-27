//  # glsl버전 4.5에 사용
#version 450

// vertex shader
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

// fragment shader로 넘겨줄 변수 -> 각 shader마다 location은 독립적
// 그래서 location이 0으로 같아도 상관없음
layout(location = 0) out vec3 fragColor;

// push constant를 적용
// 한 .vert  파일에 하나의 push constant만 사용 가능
layout(push_constant) uniform Push {
  vec2 offset;
  vec3 color;
}
push;

void main() {
  gl_Position = vec4(position + push.offset, 0.0, 1.0);
  //   fragColor = color;
}