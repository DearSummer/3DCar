#include "TPCamera.h"



TPCamera::TPCamera()
= default;


TPCamera::~TPCamera()
= default;

DirectX::XMFLOAT3 TPCamera::GetTargetPosition() const
{
	return m_target;
}

float TPCamera::GetDistance() const
{
	return m_distance;
}

float TPCamera::GetRotationX() const
{
	return m_phi;
}

float TPCamera::GetRotationY() const
{
	return m_theta;
}

void TPCamera::RotateX(float rad)
{
	m_phi -= rad;
	if (m_phi < DirectX::XM_PI / 6)
		m_phi = DirectX::XM_PI / 6;
	else if (m_phi > DirectX::XM_PIDIV2)
		m_phi = DirectX::XM_PIDIV2;
}

void TPCamera::RotateY(float rad)
{
	m_theta = DirectX::XMScalarModAngle(m_theta - rad);
}

void TPCamera::Approach(float dist)
{
	m_distance += dist;
	if (m_distance < m_minDist)
		m_distance = m_minDist;
	else if (m_distance > m_maxDist)
		m_distance = m_maxDist;

}

void TPCamera::SetRotationX(float phi)
{
	m_phi = DirectX::XMScalarModAngle(phi);
	if (m_phi < DirectX::XM_PI / 6)
		m_phi = DirectX::XM_PI / 6;
	else if (m_phi > DirectX::XM_PIDIV2)
		m_phi = DirectX::XM_PIDIV2;
}

void TPCamera::SetRotationY(float theta)
{
	m_theta = DirectX::XMScalarModAngle(theta);
}

void TPCamera::SetTarget(const DirectX::XMFLOAT3& target)
{
	m_target = target;
}

void TPCamera::SetDistance(float dist)
{
	m_distance = dist;
}

void TPCamera::SetDistanceMinMax(float minDist, float maxDist)
{
	m_minDist = minDist;
	m_maxDist = maxDist;
}

void TPCamera::UpdateViewMatrix()
{
	using namespace DirectX;

	float x = m_target.x + m_distance * sinf(m_phi) * cosf(m_theta);
	float z = m_target.z + m_distance * sinf(m_phi) * sinf(m_theta);
	float y = m_target.y + m_distance * cosf(m_phi);

	m_position = { x,y,z };

	const XMVECTOR pos = XMLoadFloat3(&m_position);
	const XMVECTOR look = XMVector3Normalize(XMLoadFloat3(&m_target) - pos);
	const XMVECTOR right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), look));
	const XMVECTOR up = XMVector3Cross(look, right);

	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_up, up);
	XMStoreFloat3(&m_look, look);

	m_view = {
		m_right.x,m_up.x,m_look.x,0.0f,
		m_right.y,m_up.y,m_look.y,0.0f,
		m_right.z,m_up.z,m_look.z,0.0f,
		-XMVectorGetX(XMVector3Dot(pos,right)),-XMVectorGetY(XMVector3Dot(pos,up)),-XMVectorGetZ(XMVector3Dot(pos,look)),1.0f
	};

}
