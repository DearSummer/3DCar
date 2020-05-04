#include "FPCamera.h"



FPCamera::FPCamera()
= default;


FPCamera::~FPCamera()
= default;

void FPCamera::SetPosition(float x, float y, float z)
{
	SetPosition(DirectX::XMFLOAT3(x, y, z));
}

void FPCamera::SetPosition(const DirectX::XMFLOAT3& v)
{
	m_position = v;
}

void FPCamera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up)
{
	LookTo(pos, DirectX::XMVectorSubtract(target,pos), up);
}

void FPCamera::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
	LookAt(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&up));
}

void FPCamera::LookTo(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR to, DirectX::FXMVECTOR up)
{
	const DirectX::XMVECTOR l = DirectX::XMVector3Normalize(to);
	const DirectX::XMVECTOR r = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, l));
	const DirectX::XMVECTOR u = DirectX::XMVector3Cross(l, r);

	DirectX::XMStoreFloat3(&m_position, pos);
	DirectX::XMStoreFloat3(&m_look, l);
	DirectX::XMStoreFloat3(&m_right, r);
	DirectX::XMStoreFloat3(&m_up, u);

}


void FPCamera::LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up)
{
	LookTo(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&to), DirectX::XMLoadFloat3(&up));
}

void FPCamera::Strafe(float d)
{
	const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_position);
	const DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&m_right);
	const DirectX::XMVECTOR dist = DirectX::XMVectorReplicate(d);
	DirectX::XMStoreFloat3(&m_position, DirectX::XMVectorMultiplyAdd(dist, right, pos));
}

void FPCamera::Straight(float d)
{
	const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_position);
	const DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&m_right);
	const DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
	const DirectX::XMVECTOR front = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, up));
	const DirectX::XMVECTOR dist = DirectX::XMVectorReplicate(d);
	DirectX::XMStoreFloat3(&m_position, DirectX::XMVectorMultiplyAdd(dist, front, pos));
}

void FPCamera::MoveForward(float d)
{
	const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_position);
	const DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&m_look);
	const DirectX::XMVECTOR dist = DirectX::XMVectorReplicate(d);
	DirectX::XMStoreFloat3(&m_position,DirectX::XMVectorMultiplyAdd(dist,look,pos));
}

void FPCamera::Pitch(float rad)
{
	const DirectX::XMMATRIX r = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_right), rad);
	const DirectX::XMVECTOR u = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_up), r);
	const DirectX::XMVECTOR look = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_look), r);

	const float cosPhi = DirectX::XMVectorGetY(look);
	if (fabs(cosPhi) > cosf(DirectX::XM_2PI / 9))
		return;


	DirectX::XMStoreFloat3(&m_up, u);
	DirectX::XMStoreFloat3(&m_look, look);
}

void FPCamera::RotateY(float rad)
{
	const DirectX::XMMATRIX r = DirectX::XMMatrixRotationY(rad);

	DirectX::XMStoreFloat3(&m_right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_right), r));
	DirectX::XMStoreFloat3(&m_up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_up), r));
	DirectX::XMStoreFloat3(&m_look, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_look), r));
}

void FPCamera::UpdateViewMatrix()
{
	using namespace DirectX;
	XMVECTOR R = XMLoadFloat3(&m_right);
	XMVECTOR L = XMLoadFloat3(&m_look);
	const XMVECTOR P = XMLoadFloat3(&m_position);

	// 保持摄像机的轴互为正交，且长度都为1
	L = XMVector3Normalize(L);
	const XMVECTOR U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L已经正交化，需要计算对应叉乘得到R
	R = XMVector3Cross(U, L);

	// 填充观察矩阵
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);
	XMStoreFloat3(&m_look, L);

	m_view = {
		m_right.x, m_up.x, m_look.x, 0.0f,
		m_right.y, m_up.y, m_look.y, 0.0f,
		m_right.z, m_up.z, m_look.z, 0.0f,
		x, y, z, 1.0f
	};

}




