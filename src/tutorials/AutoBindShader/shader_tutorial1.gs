
#version 120 
#extension GL_EXT_geometry_shader4 : enable

void main()
{
	int i;
	
    for(i=0; i< gl_VerticesIn; i++)  {
		gl_Position = gl_PositionIn[i] - vec4(1,0,0,0);
		EmitVertex();
		gl_Position = gl_PositionIn[i] + vec4(1,0,0,0);
		EmitVertex();
		gl_Position = gl_PositionIn[i] + vec4(0,1,0,0);
		EmitVertex();
	}
	EndPrimitive();
}
