#pragma once
#include "WinApp.h"
#include "Effect.h"
#include "GeometryGens.h"
#include "Camera.h"
#include "pickRay.h"
class D3DBlending : public WinApp
{
protected:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	};
public:
	

	D3DBlending(HINSTANCE hInst, std::wstring title = L"d3dLearning", int width = 1024, int height = 720);
	~D3DBlending();
	
	bool Init();
	bool Update(float delta);
	bool Render();
	
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnKeyDown(WPARAM keyPressed);
	void OnKeyUp(WPARAM keyPressed);

private:
	BasicEffect*   m_effect;
	Camera	   *   m_camera;
	pickRay    *   m_pickRay;
	bool BuildShaderResourceView();
	bool BuildBuffers();
	
	ID3D11Buffer	*m_VB;
	ID3D11Buffer	*m_IB;

	Lights::PointLight  m_pointLight;
	XMFLOAT4   fogColor;
	float	   fogStart;
	float      fogRange;
	GeoGen::MeshData m_basin;
	GeoGen::MeshData m_water;
	GeoGen::MeshData m_box;

	Lights::Material m_materialBasin;
	Lights::Material m_materialWater;
	Lights::Material m_materialBox;

	ID3D11ShaderResourceView *m_SRVBasin;
	ID3D11Resource           *m_RBasin;
	ID3D11ShaderResourceView *m_SRVBasinBottom;
	ID3D11Resource           *m_RBasinBottom;
	ID3D11ShaderResourceView *m_SRVWater;
	ID3D11Resource           *m_RWater;
	ID3D11ShaderResourceView *m_SRVBox;
	ID3D11Resource           *m_RBox;

	

	XMFLOAT4X4 m_worldBasin;
	XMFLOAT4X4 m_worldInvTransposeBasin;
	XMFLOAT4X4 m_worldViewProjBasin;
	XMFLOAT4X4 m_texTransBasin;

	XMFLOAT4X4 m_worldBasinBottom;
	XMFLOAT4X4 m_worldInvTransposeBasinBottom;
	XMFLOAT4X4 m_worldViewProjBasinBottom;
	XMFLOAT4X4 m_texTransBasinBottom;

	XMFLOAT4X4 m_worldWater;
	XMFLOAT4X4 m_worldInvTransposeWater;
	XMFLOAT4X4 m_worldViewProjWater;
	XMFLOAT4X4 m_texTransWater;

	XMFLOAT4X4 m_worldBox;
	XMFLOAT4X4 m_worldInvTransposeBox;
	XMFLOAT4X4 m_worldViewProjBox;
	XMFLOAT4X4 m_texTransBox;
	


	UINT	m_basinVStart, m_basinIStart;		//水池壁的顶点、索引开始处
	UINT	m_basinBVStart, m_basinBIStart;		//水池底的顶点、索引开始处
	UINT	m_basinICount, m_basinBICount;		//水池壁、水池底的索引个数
	UINT	m_boxVStart, m_boxIStart;			//箱子的顶点、索引开始处
	UINT	m_waterVStart, m_waterIStart;		//水面的顶点、索引开始处
	

	//鼠标控制参数
	float	m_theta, m_phy;
	float	m_radius;
	POINT	m_lastPos;
	//键盘控制参数
	int techSelected;
};

