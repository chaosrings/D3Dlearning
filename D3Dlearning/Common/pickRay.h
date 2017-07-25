#pragma once
#include "AppUtil.h"
#include <d3d11_1.h>
#include <directxmath.h>
using namespace DirectX;
class pickRay
{
public:
	pickRay(int screenWidth,int screenHeight,CXMMATRIX Proj) {
		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;
		setProjMat(Proj);
	};
	~pickRay() {};
	void setProjMat(CXMMATRIX Proj)
	{
		XMStoreFloat4x4(&m_projMat, Proj);
	}
	void setWidth(int width)
	{
		m_screenWidth = width;
	}
	void setHeight(int height)
	{
		m_screenHeight = height;
	}
	bool calViewSpaceRay(int mouseX, int mouseY)
	{
		float  vx = (2.f*mouseX/m_screenWidth - 1.f) / m_projMat._11;
		float vy = (-2.f*mouseY / m_screenHeight + 1.f) / m_projMat._22;
		m_Origin = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		m_Dir = XMFLOAT3(vx, vy, 1.f);
		return true;
	}
	void TransformRay(CXMMATRIX  T)
	{
		XMVECTOR Origin = XMLoadFloat4(&m_Origin);
		XMVECTOR Dir = XMLoadFloat3(&m_Dir);
		Origin=XMVector3TransformCoord(Origin, T);
		Dir=XMVector3TransformNormal(Dir, T);
		Dir = XMVector3Normalize(Dir);
		XMStoreFloat4(&m_Origin, Origin);
		XMStoreFloat3(&m_Dir, Dir);
	}
	void TransFromViewToLocal(CXMMATRIX targetWorld, CXMMATRIX cameraView)
	{
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(cameraView), cameraView);
		XMMATRIX invTargetWorld = XMMatrixInverse(&XMMatrixDeterminant(targetWorld), targetWorld);
		XMMATRIX transMat = invView*invTargetWorld;
		TransformRay(transMat);
	}
	bool RaySphereIntTest(float center, float radius)
	{
		XMFLOAT3 m = XMFLOAT3(m_Origin.x, m_Origin.y, m_Origin.z);
		XMFLOAT3 dir=m_Dir;
		float a = dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
		float b = 2 * (m.x*dir.x + m.y*dir.y + m.z*dir.z);
		float c= (m.x*m.x + m.y*m.y + m.z*m.z) - radius*radius;
		float discriminant = b*b - 4 * a*c;
		if (discriminant >= 0)
		{
			return true;
		}
		return false;
	}
private:
	XMFLOAT4 m_Origin;
	XMFLOAT3 m_Dir;
	XMFLOAT4X4  m_projMat;
	int m_screenWidth;
	int m_screenHeight;
};