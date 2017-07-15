#pragma once
#include "AppUtil.h"
#include "Effect.h"
#include "WinApp.h"
#include "RenderStates.h"
#include "GeometryGens.h"
#include "Lights.h"
struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
};
class MirrorDemo: public WinApp
{
public:
	
	MirrorDemo(HINSTANCE hInst, std::wstring title = L"D3Dlearing Mirror", int width = 1024, int height = 720);
	~MirrorDemo();
	bool Init();
	bool Update(float delta);
	bool Render();
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnKeyDown(WPARAM keyPressed);
	void OnKeyUp(WPARAM keyPressed);

private:
	bool BuildBuffers();
	bool BuildShaderReources();

	BlendEffect *m_effect;
	int techSelected;

	ID3D11Buffer  *m_VBPlane;
	ID3D11Buffer  *m_IBPlane;
	ID3D11Buffer  *m_VBBox;
	ID3D11Buffer  *m_IBBox;

	GeoGen::MeshData  m_box;
	
	//帧
	Lights::PointLight  m_pointLight;
	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;
	XMFLOAT4   m_fogColor;
	float      m_fogStart;
	float	   m_fogRange;
	//物体
	XMFLOAT4X4  m_worldWall;
	XMFLOAT4X4  m_worldViewProjWall;
	XMFLOAT4X4  m_worldInvTransposeWall;
	XMFLOAT4X4  m_texTransWall;

	//?
	XMFLOAT4X4  m_worldFloor;
	XMFLOAT4X4  m_worldViewProjFloor;
	XMFLOAT4X4  m_worldInvTransposeFloor;
	XMFLOAT4X4  m_texTransFloor;

	XMFLOAT4X4  m_worldBox;
	XMFLOAT4X4  m_worldViewProjBox;
	XMFLOAT4X4  m_worldInvTransposeBox;
	XMFLOAT4X4  m_texTransBox;


	XMFLOAT4X4  m_worldMirror;
	XMFLOAT4X4  m_worldViewProjMirror;
	XMFLOAT4X4  m_worldInvTransposeMirror;
	XMFLOAT4X4  m_texTransMirror;

	ID3D11ShaderResourceView  *m_SRVWall;
	ID3D11ShaderResourceView  *m_SRVFloor;
	ID3D11ShaderResourceView  *m_SRVBox;
	ID3D11ShaderResourceView	*m_SRVMirror;

	Lights::Material  m_materialWall;
	Lights::Material  m_materialFloor;
	Lights::Material  m_materialBox;
	Lights::Material  m_materialMirror;
	Lights::Material  m_materialShadow;
	float m_theta, m_phy;
	float m_radius;
	POINT m_lastPos;

	//键盘控制参数
	int dx;
	int dz;


};

