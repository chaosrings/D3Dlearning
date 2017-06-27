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

	//顶点、索引缓冲区
	ID3D11Buffer	*m_VertexBuffer;
	ID3D11Buffer	*m_IndexBuffer;

	//Effect接口
	ID3DX11Effect					*m_fx;

	//Effect全局变量
	//针对每个物体
	ID3DX11EffectMatrixVariable		*m_fxWorldViewProj;
	ID3DX11EffectMatrixVariable		*m_fxWorld;
	ID3DX11EffectMatrixVariable		*m_fxWorldInvTranspose;
	ID3DX11EffectVariable			*m_fxMaterial;
	
	//针对每一帧
	ID3DX11EffectVariable			*m_fxdirLights;
	ID3DX11EffectVariable		    *m_fxPointLight;
	ID3DX11EffectVariable			*m_fxEyePos;

	Lights::DirLight				m_lights[3];
	Lights::PointLight              m_pointLight;
	int								m_numLights;
	Lights::Material				m_matGrid;
	Lights::Material				m_matSphere;
	XMFLOAT3						m_eyePos;

	//视角、投影矩阵
	XMFLOAT4X4	m_view;
	XMFLOAT4X4	m_proj;

	//几何物体
	GeoGen::MeshData	m_grid;
	GeoGen::MeshData	m_sphere;

	//几何物体顶点、索引位置信息
	UINT	m_gridVStart, m_gridIStart;
	UINT	m_sphereVStart, m_sphereIStart;
	//几何物体世界变换矩阵
	XMFLOAT4X4	m_gridWorld;
	XMFLOAT4X4	m_sphereWorld[5];

	//鼠标控制参数
	POINT	m_lastPos;
	float	m_theta, m_phy;
	float	m_radius;
	//键盘控制参数
	
	float dx;
	float dz;
	float v;
	float v0;
	bool isJumping;
};