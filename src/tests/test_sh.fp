// SL 2012-02-08

uniform sampler2D u_Image;

varying vec2      v_TexCoord;

void main()
{
  gl_FragColor = texture( u_Image, v_TexCoord );
}

