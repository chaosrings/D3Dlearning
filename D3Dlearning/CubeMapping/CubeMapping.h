#pragma once
#include "WinApp.h"
#include "Camera.h"
#include "SkyBoxEffect.h"
#include "GeometryGens.h"
#include "RenderStates.h"
#include "Terrain.h"
class CubeMapping : public WinApp
{
private:
	struct PosVertex
	{
		XMFLOAT3 pos;
	};
	struct basic32Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	};
public:
	CubeMapping(HINSTANCE hInst, wstring title = L"SkyBox and Reflection", int width = 1024, int height = 720);
	bool Init();
	~CubeMapping();
	bool BuildBuffers();
	bool BuildShaderResourceView();
	bool BuildDynamicCubeMappingViews();
	bool updateDynamicCameras(float,float,float);
	bool Update(float delta);
	bool Render();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnKeyDown(WPARAM keyPressed);
	void OnKeyUp(WPARAM keyPressed);
private:
	SkyBoxEffect *m_skyEffect;
	BasicEffect  *m_basicEffect;
	Camera		 *m_camera;
	

	ID3D11Buffer *m_skyVB;
	ID3D11Buffer *m_skyIB;

	ID3D11Buffer *m_refVB;
	ID3D11Buffer *m_refIB;

	Lights::PointLight m_pointLight;
	//ÎïÌå
	GeoGen::MeshData m_skySphere;
	GeoGen::MeshData m_refSphere;

	Lights::Material m_materialRef;
	Lights::Material m_materialObj;
	XMFLOAT4X4  m_refWorld;
	XMFLOAT4X4 m_objWorld;
	
	ID3D11ShaderResourceView *m_SRVObjectSphere;
	ID3D11ShaderResourceView *m_SRVSkyBox;

	UINT m_cubeMapWidth = 512;
	UINT m_cubeMapHeight = 512;
	
	ID3D11RenderTargetView  *m_dynamicRTV[6];
	ID3D11DepthStencilView  *m_dynamicDSV;
	ID3D11ShaderResourceView *m_dynamicSRV;
	D3D11_VIEWPORT          m_viewport;
	D3D11_VIEWPORT			m_dynamicViewport;
	Camera					m_dynamicCameras[6];
	POINT m_lastPos;

	bool usewfRender = true;
	bool userefRender = false;
};