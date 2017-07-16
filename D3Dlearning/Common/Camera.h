#pragma once
#include <Windows.h>
#include <directxmath.h>
#include <cmath>
using namespace DirectX;
class Camera
{

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_up;
	XMFLOAT3 m_look;
	XMFLOAT3 m_right;

	float m_aspect;
	float m_fovY;
	float m_nearZ;
	float m_farZ;

	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;
public:
	Camera(int width, int height);
	Camera() {};
	~Camera() {};
	void  setPosition(float x, float y, float z) { m_position = XMFLOAT3(x, y, z); }
	void setPositionXM(XMFLOAT3 pos) { m_position = pos; }

	XMFLOAT3 getPosition() const { return m_position; }
	XMFLOAT3 getUp() const { return m_up; }
	XMFLOAT3 getLook() const { return m_look; }
	XMFLOAT3 getRight() const { return m_right; }

	float getAspect() const { return m_aspect; }
	float getFovY() const { return m_fovY; }
	float getNearZ() const { return m_nearZ; }
	float getFarZ() const { return m_farZ; }

	XMMATRIX  View() const { return XMLoadFloat4x4(&m_view); }
	XMMATRIX  Proj() const { return XMLoadFloat4x4(&m_proj); }
	XMMATRIX  viewProj() const { return XMLoadFloat4x4(&m_view)*XMLoadFloat4x4(&m_proj); }

	void setLens(float fovY, float ratioAspect, float nearZ, float farZ)
	{
		m_fovY = fovY;
		m_aspect = ratioAspect;
		m_nearZ = nearZ;
		m_farZ = farZ;
		
		XMStoreFloat4x4(&m_proj, XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ));
	}

	void Walk(float dist);
	void Strafe(float dist);
	void Pitch(float angle);
	void RotateY(float angle);
	void updateView();
};