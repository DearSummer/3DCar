#include "BasicCamera.h"


BasicCamera::BasicCamera()
	: m_position(0.0f, 0.0f, 0.0f), m_right(0.0f, 0.0f, 0.0f),
	m_up(0.0f, 0.0f, 0.0f), m_look(0.0f, 0.0f, 0.0f),
	m_nearZ(), m_narZ(), m_aspect(),m_fovY(),
	m_nearWindowHeight(), m_farWindowHeight(), m_viewPort()
{
}

BasicCamera::~BasicCamera()
= default;


DirectX::XMVECTOR BasicCamera::GetPositionXM() const
{
	return XMLoadFloat3(&m_position);
}

DirectX::XMFLOAT3 BasicCamera::GetPosition() const
{
	return m_position;
}

DirectX::XMVECTOR BasicCamera::GetRightXM() const
{
	return XMLoadFloat3(&m_right);
}

DirectX::XMFLOAT3 BasicCamera::GetRight() const
{
	return m_right;
}

DirectX::XMVECTOR BasicCamera::GetUpXM() const
{
	return XMLoadFloat3(&m_up);
}

DirectX::XMFLOAT3 BasicCamera::GetUp() const
{
	return m_up;
}

DirectX::XMVECTOR BasicCamera::GetLookXM() const
{
	return XMLoadFloat3(&m_look);
}

DirectX::XMFLOAT3 BasicCamera::GetLook() const
{
	return m_look;
}

float BasicCamera::GetNearWindowWidth() const
{
	return m_aspect * m_nearWindowHeight;
}

float BasicCamera::GetNearWindowHeight() const
{
	return m_nearWindowHeight;
}

float BasicCamera::GetFarWindowWidth() const
{
	return m_aspect * m_farWindowHeight;
}

float BasicCamera::GetFarWindowHeight() const
{
	return m_farWindowHeight;
}

DirectX::XMMATRIX BasicCamera::GetViewXM() const
{
	return XMLoadFloat4x4(&m_view);
}

DirectX::XMMATRIX BasicCamera::GetProjXM() const
{
	return XMLoadFloat4x4(&m_proj);
}

DirectX::XMMATRIX BasicCamera::GetViewProjXM() const
{
	return XMLoadFloat4x4(&m_view) * XMLoadFloat4x4(&m_proj);
}

D3D11_VIEWPORT BasicCamera::GetViewPort() const
{
	return m_viewPort;
}

void BasicCamera::SetFrustum(float fovY, float aspect, float nearZ, float farZ)
{
	m_fovY = fovY;
	m_aspect = aspect;
	m_nearZ = nearZ;
	m_narZ = farZ;

	m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * m_fovY);
	m_farWindowHeight = 2.0f * m_narZ * tanf(0.5f * m_fovY);

	XMStoreFloat4x4(&m_proj, DirectX::XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_narZ));
}

void BasicCamera::SetViewPort(const D3D11_VIEWPORT & viewPort)
{
	m_viewPort = viewPort;
}

void BasicCamera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	m_viewPort.TopLeftX = topLeftX;
	m_viewPort.TopLeftY = topLeftY;
	m_viewPort.Width = width;
	m_viewPort.Height = height;
	m_viewPort.MinDepth = minDepth;
	m_viewPort.MaxDepth = maxDepth;
}