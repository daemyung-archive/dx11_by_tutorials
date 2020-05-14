#pragma once

class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetFrameTime(float);
	void GetRotation(float&);

	void TurnLeft(bool);
	void TurnRight(bool);

private:
	float m_frameTime = 0.0f;
	float m_rotationY = 0.0f;
	float m_leftTurnSpeed = 0.0;
	float m_rightTurnSpeed = 0.0f;
};

