#version 420 core
#extension GL_NV_shader_buffer_load : enable
#extension GL_NV_gpu_shader5        : enable

in vec4  u_Color;

out vec4 o_PixColor;

uniform uint8_t *u_Buffer;

void main()
{
  // write to buffer
  int i = int(u_Color.x * 256.0) & 255;
  u_Buffer[ i ] = uint8_t(255) - uint8_t(i);
  // output color
  o_PixColor = u_Color;
}
