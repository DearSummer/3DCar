#include "Car.h"
#include "DXTrace.h"
#include "WICTextureLoader.h"


Car::Car()
	:m_forward(DirectX::XMVectorSet(0.0f,0.0f,1.0f,1.0f)),
	m_distance(0),m_rotationYDetla(DirectX::XM_PI / 90),
	m_direction(Direction::STOP),m_turn(Turn::NONE)
{
}

void Car::Init(ID3D11Device *device)
{
	ComPtr<ID3D11ShaderResourceView> pTexture;
	m_wheelVec.resize(4);
	DX_DEBUG_TRACE(DirectX::CreateWICTextureFromFile(device, L"Texture\\stone.jpg", nullptr, &pTexture));
	for (int i = 0; i < 4; i++)
	{
		m_wheelVec[i].SetBuffer(device, Geometry::CreateCylinder(0.5f, 0.5f));
		m_wheelVec[i].SetTexture(pTexture.Get());
		m_wheelVec[i].SetMaterial(m_material);
		m_wheelVec[i].SetRotation({ -DirectX::XM_PIDIV2,0.0f,DirectX::XM_PIDIV2 });
	}

	m_wheelVec[0].SetPosition({ 1.0f,-0.5f,-1.0f });
	m_wheelVec[1].SetPosition({ 1.0f,-0.5f, 1.0f });
	m_wheelVec[2].SetPosition({ -1.0f,-0.5f,-1.0f });
	m_wheelVec[3].SetPosition({ -1.0f,-0.5f,1.0f });
}

void Car::SetDirection(Direction d)
{
	m_direction = d;
}


void Car::SetTurn(Turn t)
{
	m_turn = t;
}

void Car::Draw(ID3D11DeviceContext* deviceContext, BasicEffect& effect)
{
	GameObject::Draw(deviceContext, effect);
	for(auto& wheel : m_wheelVec)
	{
		wheel.Draw(deviceContext, effect);
	}
}

void Car::SetCarMaterial(const Material& mat)
{
	SetMaterial(mat);
	for(auto& wheel : m_wheelVec)
	{
		wheel.SetMaterial(mat);
	}
}


void Car::Move()
{
	const DirectX::XMMATRIX rot =
		DirectX::XMMatrixRotationRollPitchYaw(
			0.0f,
			m_rotationYDetla * static_cast<int>(m_turn) * static_cast<int>(m_direction),
			0.0f);

	const DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(
		1.0f * static_cast<int>(m_direction) / 20,
		0.0f,
		1.0f * static_cast<int>(m_direction) / 20);

	const DirectX::XMVECTOR v = DirectX::XMVector4Transform(m_forward, rot * scale);
	m_forward = DirectX::XMVector4Transform(m_forward, rot);
	DirectX::XMFLOAT3 pos = GetPosition();
	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&pos);
	posVec = DirectX::XMVectorAdd(posVec, v);
	DirectX::XMStoreFloat3(&pos, posVec);
	SetPosition(pos);
	SetRotation({ 0.0f,GetWholeRotoation(),0.0f });

	m_distance = static_cast<int>(m_direction) * 1.0f / 20;

	DirectX::XMVECTOR wheelOffset[4];

	int i = 0;
	for (auto& wheel : m_wheelVec)
	{
		const DirectX::XMFLOAT3 wheelPos = wheel.GetPosition();
		wheelOffset[i] = DirectX::XMVectorSet(wheelPos.x, wheelPos.y, wheelPos.z, 1.0f);
		i++;
	}

	DirectX::XMMATRIX translationMat[4];
	DirectX::XMMATRIX rotationMat[4];
	for (i = 0; i < 4; i++)
	{
		DirectX::XMFLOAT3 carPos = GetPosition();
		wheelOffset[i] = DirectX::XMVector4Transform(wheelOffset[i], DirectX::XMMatrixRotationRollPitchYaw(0.0f, GetWholeRotoation(), 0.0f));
		translationMat[i] = DirectX::XMMatrixTranslationFromVector(
			DirectX::XMVectorAdd(DirectX::XMVectorAdd(wheelOffset[i], DirectX::XMLoadFloat3(&carPos)), v));
		rotationMat[i] = DirectX::XMMatrixRotationRollPitchYaw(
			-DirectX::XM_PIDIV2, 
			GetWholeRotoation(), 
			DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4 * static_cast<int>(m_turn));
		//TODO:转起来
		const DirectX::XMVECTOR wheelForward = DirectX::XMVector4Transform(m_forward, rotationMat[i]);
		const DirectX::XMVECTOR axis = DirectX::XMVector3Cross(wheelForward, { 0.0f,1.0f,0.0f });
		rotationMat[i] *= DirectX::XMMatrixRotationAxis(axis, DirectX::XM_PIDIV4 * m_distance);
		m_wheelVec[i].SetWorldMatrix(rotationMat[i] * translationMat[i]);
	}





}

float Car::GetWholeRotoation() const
{
	const DirectX::XMVECTOR v1 = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	const DirectX::XMVECTOR v2 = DirectX::XMVector3Normalize(m_forward);
	const DirectX::XMVECTOR v3 = DirectX::XMVector3Cross(v1, v2);
	const float wholeRotation = DirectX::XMVectorGetY(DirectX::XMVector3AngleBetweenNormals(v1, v2));
	return DirectX::XMVectorGetY(v3) > 0 ? wholeRotation : -wholeRotation;
}

float Car::GetDistance() const
{
	return m_distance;
}

DirectX::XMFLOAT3 Car::GetForward() const
{
	DirectX::XMFLOAT3 p{};
	DirectX::XMStoreFloat3(&p, m_forward);
	return p;
}






