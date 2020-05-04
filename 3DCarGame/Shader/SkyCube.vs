#include "SkyCube.hlsli"


VertexOut VS(VertexIn vIn)
{
    VertexOut vOut;

    float4 posH = mul(float4(vIn.pos,1.0f),g_worldViewProj);
    vOut.posH = posH.xyww;
    vOut.posL = vIn.pos;
    return vOut;
}