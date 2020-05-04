#include "Light.hlsli"

Texture2D g_tex : register(t0);
TextureCube g_texCube : register(t1);

SamplerState g_sam : register(s0);

cbuffer CBOnDrawing:register(b0)
{
	matrix g_world;
	matrix g_worldInvTranspose;
	Material g_mat;
}

cbuffer CBDrawState:register(b1)
{
	int g_isReflection;
	int g_isShadow;
	float2 g_state_pad;
}

cbuffer CBOnFrameChange:register(b2)
{
	matrix g_view;
	float3 g_eyePos;
}

cbuffer CBOnResize:register(b3)
{
	matrix g_proj;
}


cbuffer CBEnvironmentBasic : register(b4)
{
	matrix g_reflection;
	matrix g_shadow;
	matrix g_refShadow;
	DirectionalLight g_light;
}

struct VertexIn
{
	float3 pos : POSITION;
	float3 normal: NORMAL;
	float4 tex : TEXCOORD;
};

struct VertexOut
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW:NORMAL;
	float4 tex : TEXCOORD;
};


