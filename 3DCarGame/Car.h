#pragma once
#include "GameObject.h"

class Car: public GameObject
{
public:
	Car();
	~Car()= default;

	enum Direction
	{
		FORWARD = 1,
		STOP = 0,
		BACK = -1
	};

	enum Turn
	{
		LEFT = -1,
		NONE = 0,
		RIGHT = 1
	};

	void Init(ID3D11Device * context);

	void SetDirection(Direction d);
	void SetTurn(Turn t);

	void Draw(ID3D11DeviceContext* deviceContext, BasicEffect& effect) override;
	void SetCarMaterial(const Material& mat);

	void Move();
	float GetWholeRotoation() const;
	float GetDistance() const;
	DirectX::XMFLOAT3 GetForward() const;

private:

	std::vector<GameObject> m_wheelVec;

	DirectX::XMVECTOR m_forward;
	float m_distance,m_rotationYDetla;
	Direction m_direction;
	Turn m_turn;
};

