#pragma once
#include "BasicCamera.h"

class TPCamera :public BasicCamera
{
public:
	TPCamera();
	~TPCamera();

	// 获取当前跟踪物体的位置
	DirectX::XMFLOAT3 GetTargetPosition() const;
	// 获取与物体的距离
	float GetDistance() const;
	// 获取绕X轴的旋转方向
	float GetRotationX() const;
	// 获取绕Y轴的旋转方向
	float GetRotationY() const;
	// 绕物体垂直旋转(注意上下视野角度Phi限制在[pi/6, pi/2])
	void RotateX(float rad);
	// 绕物体水平旋转
	void RotateY(float rad);
	// 拉近物体
	void Approach(float dist);
	// 设置初始绕X轴的弧度(注意上下视野角度Phi限制在[pi/6, pi/2])
	void SetRotationX(float phi);
	// 设置初始绕Y轴的弧度
	void SetRotationY(float theta);
	// 设置并绑定待跟踪物体的位置
	void SetTarget(const DirectX::XMFLOAT3& target);
	// 设置初始距离
	void SetDistance(float dist);
	// 设置最小最大允许距离
	void SetDistanceMinMax(float minDist, float maxDist);
	// 更新观察矩阵
	void UpdateViewMatrix() override;

private:
	DirectX::XMFLOAT3 m_target;
	float m_distance;
	// 最小允许距离，最大允许距离
	float m_minDist, m_maxDist;
	// 以世界坐标系为基准，当前的旋转角度
	float m_theta;
	float m_phi;

};

