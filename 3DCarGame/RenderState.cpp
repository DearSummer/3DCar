﻿#include "RenderState.h"
#include "DXTrace.h"

Microsoft::WRL::ComPtr<ID3D11RasterizerState> RenderState::s_wireframe = nullptr;
Microsoft::WRL::ComPtr<ID3D11RasterizerState> RenderState::s_noCull = nullptr;
Microsoft::WRL::ComPtr<ID3D11RasterizerState> RenderState::s_cullClockWise = nullptr;

Microsoft::WRL::ComPtr<ID3D11SamplerState> RenderState::s_linearWrap = nullptr;
Microsoft::WRL::ComPtr<ID3D11SamplerState> RenderState::s_anistropWrap = nullptr;

Microsoft::WRL::ComPtr<ID3D11BlendState> RenderState::s_noColorWrite = nullptr;
Microsoft::WRL::ComPtr<ID3D11BlendState> RenderState::s_transparent = nullptr;
Microsoft::WRL::ComPtr<ID3D11BlendState> RenderState::s_alphaToCoverage = nullptr;

Microsoft::WRL::ComPtr<ID3D11DepthStencilState> RenderState::s_lessEqual = nullptr;
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> RenderState::s_writeStencil = nullptr;		
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> RenderState::s_drawWithStencil = nullptr;	
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> RenderState::s_noDoubleBlend = nullptr;	
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> RenderState::s_noDepthTest = nullptr;		
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> RenderState::s_noDepthWrite = nullptr;		


void RenderState::Init(ID3D11Device* device)
{
	if (IsInit())
		return;
	// ******************
	// 初始化光栅化器状态
	//
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof rasterizerDesc);

	// 线框模式
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;
	DX_DEBUG_TRACE(device->CreateRasterizerState(&rasterizerDesc, &s_wireframe));

	// 无背面剔除模式
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;
	DX_DEBUG_TRACE(device->CreateRasterizerState(&rasterizerDesc, &s_noCull));

	// 顺时针剔除模式
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthClipEnable = true;
	DX_DEBUG_TRACE(device->CreateRasterizerState(&rasterizerDesc, &s_cullClockWise));

	// ******************
	// 初始化采样器状态
	//
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof sampDesc);

	// 线性过滤模式
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	DX_DEBUG_TRACE(device->CreateSamplerState(&sampDesc, &s_linearWrap));

	// 各向异性过滤模式
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MaxAnisotropy = 4;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	DX_DEBUG_TRACE(device->CreateSamplerState(&sampDesc, &s_anistropWrap));

	// ******************
	// 初始化混合状态
	//
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	auto& rtDesc = blendDesc.RenderTarget[0];
	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.IndependentBlendEnable = false;
	rtDesc.BlendEnable = false;
	rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	DX_DEBUG_TRACE(device->CreateBlendState(&blendDesc, &s_alphaToCoverage));

	// 透明混合模式
	// Color = SrcAlpha * SrcColor + (1 - SrcAlpha) * DestColor 
	// Alpha = SrcAlpha
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	rtDesc.BlendEnable = true;
	rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

	DX_DEBUG_TRACE(device->CreateBlendState(&blendDesc, &s_transparent));

	// 无颜色写入混合模式
	// Color = DestColor
	// Alpha = DestAlpha
	rtDesc.SrcBlend = D3D11_BLEND_ZERO;
	rtDesc.DestBlend = D3D11_BLEND_ONE;
	rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
	rtDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	DX_DEBUG_TRACE(device->CreateBlendState(&blendDesc, &s_noColorWrite));

	// ******************
	// 初始化深度/模板状态
	//
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	dsDesc.StencilEnable = false;

	DX_DEBUG_TRACE(device->CreateDepthStencilState(&dsDesc, &s_lessEqual));

	// 镜面标记深度/模板状态
	// 这里不写入深度信息
	// 无论是正面还是背面，原来指定的区域的模板值都会被写入StencilRef
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DX_DEBUG_TRACE(device->CreateDepthStencilState(&dsDesc, &s_writeStencil));

	// 反射绘制深度/模板状态
	// 由于要绘制反射镜面，需要更新深度
	// 仅当镜面标记模板值和当前设置模板值相等时才会进行绘制
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	DX_DEBUG_TRACE(device->CreateDepthStencilState(&dsDesc, &s_drawWithStencil));

	// 无二次混合深度/模板状态
	// 允许默认深度测试
	// 通过自递增使得原来StencilRef的值只能使用一次，实现仅一次混合
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	DX_DEBUG_TRACE(device->CreateDepthStencilState(&dsDesc, &s_noDoubleBlend));

	// 关闭深度测试的深度/模板状态
	// 若绘制非透明物体，务必严格按照绘制顺序
	// 绘制透明物体则不需要担心绘制顺序
	// 而默认情况下模板测试就是关闭的
	dsDesc.DepthEnable = false;
	dsDesc.StencilEnable = false;
	DX_DEBUG_TRACE(device->CreateDepthStencilState(&dsDesc, &s_noDepthTest));


	// 进行深度测试，但不写入深度值的状态
	// 若绘制非透明物体时，应使用默认状态
	// 绘制透明物体时，使用该状态可以有效确保混合状态的进行
	// 并且确保较前的非透明物体可以阻挡较后的一切物体
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	DX_DEBUG_TRACE(device->CreateDepthStencilState(&dsDesc, &s_noDepthWrite));
}

bool RenderState::IsInit()
{
	return s_wireframe != nullptr;
}
