uniform sampler2D tex;

uniform vec4      color;

void main()
{
  gl_FragColor = color*texture2D(tex,gl_TexCoord[0].xy);
}
