#pragma once
#include "WinApp.h"
#include "AppUtil.h"
#include "GeometryGens.h"
#include "Lights.h"
#include <d3dx11effect.h>
#include <rpcasync.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
};

class D3Dlight :public WinApp
{
public:
	D3Dlight(HINSTANCE hInst, std::wstring title = L"d3dLearning", int width = 1024, int height = 720);
	~D3Dlight();
	bool Init();
	bool Update(float delta);
	bool Render();

	 void OnMouseDown(WPARAM btnState, int x, int y);
	 void OnMouseUp(WPARAM btnState, int x, int y);
	 void OnMouseMove(WPARAM btnState, int x, int y);
	 void OnKeyDown(WPARAM keyPressed);
	 void OnKeyUp(WPARAM keyPressed);
private:
	bool BuildFX();
	bool BuildInputLayout();
	bool BuildBuffers();

private:
	ID3D11InputLayout	*m_inputLayout;

	//���㡢����������
	ID3D11Buffer	*m_VertexBuffer;
	ID3D11Buffer	*m_IndexBuffer;

	//Effect�ӿ�
	ID3DX11Effect					*m_fx;

	//Effectȫ�ֱ���
	//���ÿ������
	ID3DX11EffectMatrixVariable		*m_fxWorldViewProj;
	ID3DX11EffectMatrixVariable		*m_fxWorld;
	ID3DX11EffectMatrixVariable		*m_fxWorldInvTranspose;
	ID3DX11EffectVariable			*m_fxMaterial;
	
	//���ÿһ֡
	ID3DX11EffectVariable			*m_fxdirLights;
	ID3DX11EffectVariable		    *m_fxPointLight;
	ID3DX11EffectVariable			*m_fxEyePos;

	Lights::DirLight				m_lights[3];
	Lights::PointLight              m_pointLight;
	int								m_numLights;
	Lights::Material				m_matGrid;
	Lights::Material				m_matSphere;
	XMFLOAT3						m_eyePos;

	//�ӽǡ�ͶӰ����
	XMFLOAT4X4	m_view;
	XMFLOAT4X4	m_proj;

	//��������
	GeoGen::MeshData	m_grid;
	GeoGen::MeshData	m_sphere;

	//�������嶥�㡢����λ����Ϣ
	UINT	m_gridVStart, m_gridIStart;
	UINT	m_sphereVStart, m_sphereIStart;
	//������������任����
	XMFLOAT4X4	m_gridWorld;
	XMFLOAT4X4	m_sphereWorld[5];

	//�����Ʋ���
	POINT	m_lastPos;
	float	m_theta, m_phy;
	float	m_radius;
	//���̿��Ʋ���
	
	float dx;
	float dz;
	float v;
	float v0;
	bool isJumping;
};