// # glsl버전 4.5에 사용
#version 450

//  vec2 배열을 정의, 2차원 좌표를 저장 -> 삼각형의 세 정점의 좌표를 저장
vec2 positions[3] = vec2[](vec2(0.0, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5));

void main() {
  //  각 정점의 인덱스를 나타내는 내장 변수
  // gl_Position은 4차원 벡터로 저장
  // 3,4번째 값은 z와 w값으로, z는 깊이값, w는 화면에 가까운 정도를 나타냄
  gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}