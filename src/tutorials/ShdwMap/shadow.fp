varying vec3 v_Normal;
varying vec4 v_ShadowPos;

uniform sampler2D u_ShadowMap;

void main()
{
  float z         = v_ShadowPos.z  / v_ShadowPos.w;
  float z_shdwmap = texture2DProj(u_ShadowMap,v_ShadowPos);
  
  float  shdw     = (z > z_shdwmap) ? 0 : 1;
  
  float3 nrm      = normalize(v_Normal);
  gl_FragColor    = shdw * nrm.z;
}
