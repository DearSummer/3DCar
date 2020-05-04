#pragma once
#include "BasicCamera.h"

class FPCamera : public BasicCamera
{
public:
	FPCamera();
	~FPCamera() override;

	// 设置摄像机位置
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);
	// 设置摄像机的朝向
	void XM_CALLCONV LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
	void XM_CALLCONV LookTo(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR to, DirectX::FXMVECTOR up);
	void LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up);
	// 平移
	void Strafe(float d);
	// 直行
	void Straight(float d);
	// 前进
	void MoveForward(float d);
	// 上下观察
	void Pitch(float rad);
	// 左右观察
	void RotateY(float rad);


	// 更新观察矩阵
	void UpdateViewMatrix() override;
};

