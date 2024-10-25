// # glsl버전 4.5에 사용
#version 450

layout(location = 0) out vec4 outColor;

// vertex shader과 다르게 내장된 출력 변수가 없으므로 직접 main 선언
void main() {
  //  R, G, B, A(빨강, 초록, 파랑, 알파) 색상 채널 값
  outColor = vec4(1.0, 0.0, 1.0, 1.0);
}