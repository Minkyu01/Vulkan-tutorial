// // # glsl버전 4.5에 사용
// #version 450

// //  vec2 배열을 정의, 2차원 좌표를 저장 -> 삼각형의 세 정점의 좌표를 저장
// // vec2 positions[3] = vec2[](vec2(0.0, -0.5), vec2(0.5, 0.5), vec2(-0.5,
// 0.5)); layout(location = 0) in vec2 position;

// void main() { gl_Position = vec4(position, 0.0, 1.0); }

#version 450

layout(location = 0) in vec2 position;

void main() { gl_Position = vec4(position, 0.0, 1.0); }