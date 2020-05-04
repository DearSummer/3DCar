TextureCube g_texCube : register(t0);
SamplerState g_sam : register(s0);


cbuffer OnFrameChange:register(b0)
{
    matrix g_worldViewProj;
}

struct VertexIn
{
    float3 pos:POSITION;
};

struct VertexOut
{
    float4 posH:SV_POSITION;
    float3 posL:POSITION;
};
