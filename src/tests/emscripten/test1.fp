precision mediump float;

varying vec4 v_color;
varying vec2 v_tex;
uniform sampler2D u_tex;

void main()
{
	gl_FragColor = texture2D(u_tex,v_tex);
}
