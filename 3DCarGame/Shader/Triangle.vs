#include "Basic.hlsli"

// 顶点着色器
VertexOut VS(VertexIn vIn)
{
    VertexOut vOut;
	matrix viewProj = mul(g_view,g_proj);
    float4 posW = mul(float4(vIn.pos,1.0f),g_world);
	float3 normalW = mul(vIn.normal,(float3x3) g_worldInvTranspose);

	[flatten]
	if(g_isReflection)
	{
		posW = mul(posW,g_reflection);
		normalW = mul(normalW,(float3x3) g_reflection);
	}

	[flatten]
	if(g_isShadow)
	{
		posW = (g_isReflection ? mul(posW,g_refShadow) : mul(posW,g_shadow));
	}

	vOut.posH = mul(posW,viewProj);
	vOut.posW = posW.xyz;
	vOut.normalW = normalW;
	vOut.tex = vIn.tex;
    return vOut;
}
