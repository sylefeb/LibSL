#ifndef _FX_IMAGE_PROCESSING_
#define _FX_IMAGE_PROCESSING_

#define FX_IMAGE_PROCESSING_SHADER_HEADER \
    struct VS_OUTPUT \
    { \
      float4 Pos         : POSITION; \
      float2 Tex         : TEXCOORD1; \
    }; \
    struct VS_INPUT \
    { \
        float4 Pos       : POSITION; \
        float2 Tex       : TEXCOORD1; \
    }; \
    float4 Viewport; \
    float2 DestRegionCoord; \
    float2 DestRegionSize; \
    float2 InvDestRegionSize; \
    VS_OUTPUT vs_image_processing(VS_INPUT In) \
    { \
      VS_OUTPUT o; \
      o.Pos = float4(float2(In.Pos.x,-In.Pos.y) + float2(-1.0,1.0) / Viewport.zw ,0.0,1.0); \
      o.Tex = DestRegionCoord + In.Tex*DestRegionSize; \
      return (o); \
    }

#define FX_IMAGE_PROCESSING_SHADER_HEADER_STRING "\
    struct VS_OUTPUT \
    { \
      float4 Pos         : POSITION; \
      float2 Tex         : TEXCOORD1; \
    }; \
    struct VS_INPUT \
    { \
        float4 Pos       : POSITION; \
        float2 Tex       : TEXCOORD1; \
    }; \
    float4 Viewport; \
    float2 DestRegionCoord; \
    float2 DestRegionSize; \
    float2 InvDestRegionSize; \
    VS_OUTPUT vs_image_processing(VS_INPUT In) \
    { \
      VS_OUTPUT o; \
      o.Pos = float4(float2(In.Pos.x,-In.Pos.y) + float2(-1.0,1.0) / Viewport.zw ,0.0,1.0); \
      o.Tex = DestRegionCoord + In.Tex*DestRegionSize; \
      return (o); \
    }"

// TODO: footer ps_2_0 / ps_3_0

#define FX_IMAGE_PROCESSING_SHADER_FOOTER \
    technique t_main \
    { \
      pass P0 \
      { \
        VertexShader = compile vs_2_0 vs_image_processing(); \
  	    PixelShader  = compile ps_3_0 ps_main(); \
      } \
    }

#define FX_IMAGE_PROCESSING_SHADER_FOOTER_STRING "\
    technique t_main \
    { \
      pass P0 \
      { \
        VertexShader = compile vs_2_0 vs_image_processing(); \
  	    PixelShader  = compile ps_3_0 ps_main(); \
      } \
    }"

#endif
