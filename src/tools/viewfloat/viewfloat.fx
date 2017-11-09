
Texture Tex;

float MinValue;
float MaxValue;

float Gamma;
float ExpCompensation;

sampler2D S_Tex = sampler_state {
  Texture   = (Tex);
  MinFilter = None;
  MipFilter = None;
  MagFilter = Point;
};

struct VS_INPUT
{
  float4 Pos : POSITION;
  float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT
{
  float4 Pos : POSITION;
  float2 Tex : TEXCOORD0;
};

VS_OUTPUT vs_quad(VS_INPUT IN)
{
  VS_OUTPUT OUT;
  
  OUT.Pos = float4(IN.Pos.xy,0,1);
  OUT.Tex = IN.Tex;
  return OUT;
}

float4 ps_linear(float2 uv : TEXCOORD0) : COLOR0
{
  return float4( ((tex2D(S_Tex,uv) - MinValue) / (MaxValue - MinValue)).xyz , 1);
  
  /*
  float v = tex2D(S_Tex,uv);
  float4 clr = 0;
  if ( v > 0 && v <= MinValue ) {
    clr = 1;
  } else if ( v <= 0 && -v <= MinValue ) {
    clr = 0.5;
  }
  return clr;
  */
}

float4 ps_tonemap(float2 uv : TEXCOORD0) : COLOR0
{
  return float4( (pow( tex2D(S_Tex,uv) * pow(2.0,ExpCompensation) , 1.0 / Gamma)).xyz , 1);
}

technique t_linear {
  pass P0 {
    VertexShader = compile vs_3_0 vs_quad();
    PixelShader  = compile ps_3_0 ps_linear();
  }
}

technique t_tonemap {
  pass P0 {
    VertexShader = compile vs_3_0 vs_quad();
    PixelShader  = compile ps_3_0 ps_tonemap();
  }
}
