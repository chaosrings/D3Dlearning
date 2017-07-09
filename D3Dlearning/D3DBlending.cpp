#include "D3DBlending.h"

D3DBlending::D3DBlending(HINSTANCE hInst, wstring title, int width, int height)
	:WinApp(hInst, title, width, height), m_VB(nullptr), m_IB(nullptr),
	m_SRVBasin(nullptr), m_SRVBasinBottom(nullptr), m_SRVWater(nullptr), m_SRVBox(nullptr),
	m_theta(XM_PI*1.5f),
	m_phy(XM_PI * 0.4f),
	m_radius(20.f)
{
	m_effect = new BlendEffect();

	m_pointLight.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f);
	m_pointLight.att = XMFLOAT3(0.01f, 0.025f, 0.025f);
	m_pointLight.pos = XMFLOAT3(0.f, 6.f, 0.f);
	m_pointLight.range = 50;

	m_materialBasin.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialBasin.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	m_materialBasin.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);

	m_materialBox.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialBox.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	m_materialBox.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);

	m_materialWater.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialWater.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	m_materialWater.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_worldBasin, I);
	XMStoreFloat4x4(&m_worldInvTransposeBasin, InverseTranspose(I));
	XMStoreFloat4x4(&m_worldBasinBottom, I);
	XMStoreFloat4x4(&m_worldInvTransposeBasinBottom, InverseTranspose(I));
	
	I= XMMatrixTranslation(0.f, -1.f, 0.f);
	XMStoreFloat4x4(&m_worldBox, I);
	XMStoreFloat4x4(&m_worldInvTransposeBox, InverseTranspose(I));

	XMStoreFloat4x4(&m_worldWater, I);
	XMStoreFloat4x4(&m_worldInvTransposeWater, InverseTranspose(I));

	XMMATRIX texTransBasin = XMMatrixScaling(10.f, 1.f, 1.f);
	XMMATRIX texTransWater = XMMatrixScaling(5.f, 5.f, 1.f);
	//ÎÆÀí×ø±ê±ä»»¾ØÕó
	XMStoreFloat4x4(&m_texTransBasin, texTransBasin);
	XMStoreFloat4x4(&m_texTransBasinBottom, XMMatrixIdentity());
	XMStoreFloat4x4(&m_texTransWater, texTransWater);
	XMStoreFloat4x4(&m_texTransBox, XMMatrixIdentity());
}