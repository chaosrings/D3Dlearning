#pragma once
#include "WinApp.h"
#include "Camera.h"
#include "SkyBoxEffect.h"
#include "GeometryGens.h"
#include "RenderStates.h"
class CubeMapping : public WinApp
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
	};

public:
	CubeMapping(HINSTANCE hInst, wstring title = L"SkyBox", int width = 1024, int height = 720);
	bool Init();
	~CubeMapping();
	bool BuildBuffers();
	bool BuildShaderResourceView();
	bool Update(float delta);
	bool Render();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnKeyDown(WPARAM keyPressed);
	void OnKeyUp(WPARAM keyPressed);
private:
	SkyBoxEffect *m_effect;
	Camera		 *m_camera;

	ID3D11Buffer *m_VB;
	ID3D11Buffer *m_IB;


	//ÎïÌå
	GeoGen::MeshData m_skySphere;
	ID3D11ShaderResourceView *m_SRVSkyBox;
	POINT m_lastPos;
};