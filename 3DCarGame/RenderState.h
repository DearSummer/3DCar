#pragma once
#include <wrl/client.h>
#include <d3d11.h>
class RenderState
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	RenderState() =default;
	~RenderState() = default;

	static void Init(ID3D11Device *device);
	static bool IsInit();

	static ComPtr<ID3D11RasterizerState> s_wireframe;  //光栅化 线框
	static ComPtr<ID3D11RasterizerState> s_noCull;     //光栅化 无背裁剪
	static ComPtr<ID3D11RasterizerState> s_cullClockWise; //光栅化 顺时针裁剪

	static ComPtr<ID3D11SamplerState> s_linearWrap;  // 采样器 线性过滤
	static ComPtr<ID3D11SamplerState> s_anistropWrap;// 采样器 各项异性过滤

	static ComPtr<ID3D11BlendState> s_noColorWrite;  // 混合模式 不写入颜色
	static ComPtr<ID3D11BlendState> s_transparent;   //混合模式 混合透明
	static ComPtr<ID3D11BlendState> s_alphaToCoverage;  //混合模式 alpha-to-converage

	static ComPtr<ID3D11DepthStencilState> s_lessEqual;          //深度/模板状态：允许深度一致的情况下进行像素的替换
	static ComPtr<ID3D11DepthStencilState> s_writeStencil;		// 深度/模板状态：写入模板值
	static ComPtr<ID3D11DepthStencilState> s_drawWithStencil;	// 深度/模板状态：对指定模板值的区域进行绘制
	static ComPtr<ID3D11DepthStencilState> s_noDoubleBlend;	     // 深度/模板状态：无二次混合区域
	static ComPtr<ID3D11DepthStencilState> s_noDepthTest;		// 深度/模板状态：关闭深度测试
	static ComPtr<ID3D11DepthStencilState> s_noDepthWrite;		// 深度/模板状态：仅深度测试，不写入深度值


};

