
uniform vec4  u_Color; // tweak rgba
uniform float u_Scale; // tweak scalar min='0.0' max='1.0' step = '0.1'

void main()
{
	gl_FragColor = u_Color * u_Scale;
}
