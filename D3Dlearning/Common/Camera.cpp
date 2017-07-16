#include "Camera.h"

Camera::Camera(int width, int height):
	m_position(0.f,0.f,0.f),
	m_up(0.f,1.f,0.f),m_look(0.f,0.f,1.f),m_right(1.f,0.f,0.f),
	m_fovY(XM_PI*0.25f),m_nearZ(1.f),m_farZ(1000.f)
{
	m_aspect = 1.f*width / height;

	XMStoreFloat4x4(&m_view, XMMatrixIdentity());
	XMStoreFloat4x4(&m_proj, XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ));
}

void Camera::Walk(float dist)  //向前向后
{
	XMVECTOR look = XMLoadFloat3(&m_look);
	XMVECTOR pos = XMLoadFloat3(&m_position);

	pos += look*XMVectorReplicate(dist);
	XMStoreFloat3(&m_position, pos);
}
void Camera::Strafe(float dist)  //左右移动
{
	XMVECTOR right = XMLoadFloat3(&m_right);
	XMVECTOR pos = XMLoadFloat3(&m_position);
	pos += right*XMVectorReplicate(dist);
	XMStoreFloat3(&m_position, pos);
}
void Camera::Pitch(float angle)//视角向上向下
{
	XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);
	
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotation));  //将up和look向量绕right旋转更新
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), rotation));

}
void Camera::RotateY(float angle) //视角左右移动
{
	XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(&m_up), angle);

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotation));  //将right和look向量绕up向量旋转更新
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), rotation));
}

void Camera::updateView()
{
	XMVECTOR r = XMLoadFloat3(&m_right);
	XMVECTOR u = XMLoadFloat3(&m_up);
	XMVECTOR l = XMLoadFloat3(&m_look);
	XMVECTOR p = XMLoadFloat3(&m_position);

	r = XMVector3Normalize(XMVector3Cross(u, l));
	u = XMVector3Normalize(XMVector3Cross(l, r));
	l = XMVector3Normalize(l);

	float x = -XMVectorGetX(XMVector3Dot(p, r));
	float y = -XMVectorGetX(XMVector3Dot(p, u));
	float z = -XMVectorGetX(XMVector3Dot(p, l));

	XMStoreFloat3(&m_right, r);
	XMStoreFloat3(&m_up, u);
	XMStoreFloat3(&m_look, l);
	XMStoreFloat3(&m_position, p);
	
	//XMStoreFloat4x4(&m_view, XMMatrixLookAtLH(p, l, u));
	m_view(0, 0) = m_right.x;	m_view(0, 1) = m_up.x;	m_view(0, 2) = m_look.x;	m_view(0, 3) = 0;
	m_view(1, 0) = m_right.y;	m_view(1, 1) = m_up.y;	m_view(1, 2) = m_look.y;	m_view(1, 3) = 0;
	m_view(2, 0) = m_right.z;	m_view(2, 1) = m_up.z;	m_view(2, 2) = m_look.z;	m_view(2, 3) = 0;
	m_view(3, 0) = x;			m_view(3, 1) = y;		m_view(3, 2) = z;			m_view(3, 3) = 1;
}