#version 430
#extension GL_NV_shader_buffer_load : enable
#extension GL_NV_gpu_shader5        : enable

uniform uint8_t *u_Buffer;

layout (local_size_x = 16) in;

// gl_LocalInvocationID
// gl_GlobalInvocationID 
// gl_WorkGroupID
// gl_NumWorkGroups
// gl_WorkGroupSize

void main()
{
  u_Buffer[ gl_GlobalInvocationID.x ] = uint8_t( gl_LocalInvocationID.x );
}
