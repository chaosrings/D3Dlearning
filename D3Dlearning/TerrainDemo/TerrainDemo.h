#pragma once
#include "Terrain.h"
#include "WinApp.h"
#include <d3dx11effect.h>
#include "Effect.h"
#include "Camera.h"
#include "RenderStates.h"
#include "DDSTextureLoader.h"
class TerrainDemo : public WinApp
{
public:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	};
	TerrainDemo(HINSTANCE hInst, wstring title = L"TerrainDemo", int width = 1024, int height = 720);
	~TerrainDemo();
	bool Init();
	bool BuildBuffers();
	bool BuildSRV();
	bool Update(float delta);
	bool Render();
	void OnMouseDown(WPARAM btnState, int x, int y) {
		m_lastPos.x = x;
		m_lastPos.y = y;
		SetCapture(m_hWnd);
	}
	void OnMouseUp(WPARAM btnState, int x, int y)
	{
		ReleaseCapture();
	}
	void OnMouseMove(WPARAM btnState, int x, int y)
	{
		if ((btnState & MK_LBUTTON) != 0)
		{
			float dx = XMConvertToRadians(0.25f * (x - m_lastPos.x));
			float dy = XMConvertToRadians(0.25f * (y - m_lastPos.y));
			m_camera->Pitch(dy);
			m_camera->RotateY(dx);
		}

		m_lastPos.x = x;
		m_lastPos.y = y;
	}
	void OnKeyDown(WPARAM keyPressed) {};
	void OnKeyUp(WPARAM keyPressed) {};

private:
	BasicEffect *m_basicEffect;
	Camera      *m_camera;
	Lights::PointLight  m_pointLight;
	Terrain     *m_terrain;
	ID3D11Buffer   *m_VB;
	ID3D11Buffer   *m_IB;

	XMFLOAT4X4    m_worldTerrain;
	Lights::Material  m_materialTerrain;
	ID3D11ShaderResourceView  *m_SRVTerrain;

	POINT m_lastPos;
	
};