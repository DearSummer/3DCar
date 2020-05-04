#include "DXUtil.h"
#include <d3dcompiler.h>
#include <string>
#include "WICTextureLoader.h"


struct ID3D11DeviceContext;
struct ID3D11Device;

HRESULT DXUtil::CreateShaderFromFile(
	const WCHAR* csoFileNameOut, 
	const WCHAR* hlslFileName, 
	LPCSTR entryPoint, LPCSTR shaderModel, 
	ID3DBlob** ppBlobOut)
{
	HRESULT hResult;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob *errorBlob = nullptr;
	hResult = D3DCompileFromFile(
		hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, shaderModel, shaderFlags, 0, ppBlobOut, &errorBlob);

	if(FAILED(hResult))
	{
		if(errorBlob)
		{
			OutputDebugStringA(static_cast<const char *>(errorBlob->GetBufferPointer()));
		}
		SafeDelete(errorBlob);
		return hResult;
	}

	if(csoFileNameOut)
	{
		return D3DWriteBlobToFile(*ppBlobOut, csoFileNameOut, true);
	}

	return hResult;
}

HRESULT DXUtil::CreateWICTexture2DCubeFromFile(
	ID3D11Device * d3dDevice, 
	ID3D11DeviceContext * d3dDeviceContext, 
	const std::vector<std::wstring>& cubeMapFileNames, 
	ID3D11Texture2D ** textureArray, 
	ID3D11ShaderResourceView ** textureCubeView)
{
	// 检查设备与设备上下文是否非空
	// 文件名数目需要不小于6
	// 纹理数组和资源视图只要有其中一个非空即可
	UINT arraySize = static_cast<UINT>(cubeMapFileNames.size());

	if (!d3dDevice || !d3dDeviceContext || arraySize < 6 || !(textureArray || textureCubeView))
		return E_INVALIDARG;

	// ******************
	// 读取纹理
	//

	HRESULT hResult;
	std::vector<ID3D11Texture2D*> srcTexVec(arraySize, nullptr);
	std::vector<ID3D11ShaderResourceView*> srcTexSRVVec(arraySize, nullptr);
	std::vector<D3D11_TEXTURE2D_DESC> texDescVec(arraySize);

	for (UINT i = 0; i < arraySize; ++i)
	{
		// 该资源用于GPU复制
		hResult = DirectX::CreateWICTextureFromFile(d3dDevice,
			nullptr,
			cubeMapFileNames[i].c_str(),
			reinterpret_cast<ID3D11Resource**>(&srcTexVec[i]),
			nullptr);

		// 文件未打开
		if (hResult != S_OK)
			return hResult;
		// 读取创建好的纹理信息
		srcTexVec[i]->GetDesc(&texDescVec[i]);

		// 需要检验所有纹理的mipLevels，宽度和高度，数据格式是否一致，
		// 若存在数据格式不一致的情况，请使用dxtex.exe(DirectX Texture Tool)
		// 将所有的图片转成一致的数据格式
		if (texDescVec[i].MipLevels != texDescVec[0].MipLevels || texDescVec[i].Width != texDescVec[0].Width ||
			texDescVec[i].Height != texDescVec[0].Height || texDescVec[i].Format != texDescVec[0].Format)
		{
			for (UINT j = 0; j < i; ++j)
			{
				SafeDelete(srcTexVec[j]);
				SafeDelete(srcTexSRVVec[j]);
			}
			return E_FAIL;
		}
	}

	// ******************
	// 创建纹理数组
	//
	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texDescVec[0].Width;
	texArrayDesc.Height = texDescVec[0].Height;
	texArrayDesc.MipLevels = 1;
	texArrayDesc.ArraySize = arraySize;
	texArrayDesc.Format = texDescVec[0].Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;	// 允许从中创建TextureCube

	ID3D11Texture2D* texArray = nullptr;
	hResult = d3dDevice->CreateTexture2D(&texArrayDesc, nullptr, &texArray);

	if (FAILED(hResult))
	{
		for (UINT i = 0; i < arraySize; ++i)
		{
			SafeDelete(srcTexVec[i]);
			SafeDelete(srcTexSRVVec[i]);
		}

		return hResult;
	}

	// ******************
	// 将原纹理的所有子资源拷贝到该数组中
	//
	texArray->GetDesc(&texArrayDesc);

	for (UINT i = 0; i < arraySize; ++i)
	{
		for (UINT j = 0; j < texArrayDesc.MipLevels; ++j)
		{
			d3dDeviceContext->CopySubresourceRegion(
				texArray,
				D3D11CalcSubresource(j, i, texArrayDesc.MipLevels),
				0, 0, 0,
				srcTexVec[i],
				j,
				nullptr);
		}
	}

	// ******************
	// 创建立方体纹理的SRV
	//
	if (textureCubeView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MostDetailedMip = 0;
		viewDesc.TextureCube.MipLevels = texArrayDesc.MipLevels;

		hResult = d3dDevice->CreateShaderResourceView(texArray, &viewDesc, textureCubeView);
	}

	// 检查是否需要纹理数组
	if (textureArray)
	{
		*textureArray = texArray;
	}
	else
	{
		SafeDelete(texArray);
	}

	// 释放所有资源
	for (UINT i = 0; i < arraySize; ++i)
	{
		SafeDelete(srcTexVec[i]);
		SafeDelete(srcTexSRVVec[i]);
	}

	return hResult;
}


