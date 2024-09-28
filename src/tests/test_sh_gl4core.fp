#version 410

uniform sampler2D u_Image;

in  vec2 v_TexCoord;
out vec4 color;

void main()
{
  color = texture( u_Image, v_TexCoord );
}
