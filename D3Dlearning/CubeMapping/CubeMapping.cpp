#include "CubeMapping.h"

CubeMapping::CubeMapping(HINSTANCE hInst, wstring title, int width, int height) :
	WinApp(hInst, title, width, height),
	m_camera(nullptr), m_effect(nullptr),
	m_VB(nullptr), m_IB(nullptr),
	m_SRVSkyBox(nullptr)
{

	m_effect = new SkyBoxEffect();
	m_camera = new Camera(width, height);
}
CubeMapping::~CubeMapping()
{
	if (m_camera != nullptr)
		delete m_camera;
	if (m_effect != nullptr)
		delete m_effect;
	SafeRelease(m_VB);
	SafeRelease(m_IB);
	SafeRelease(m_SRVSkyBox);
}
bool CubeMapping::Init()
{
	if (!WinApp::Init())
		return false;
	if (!RenderStates::InitRenderStates(m_d3dDevice))
		return false;
	if (!BuildBuffers())
		return false;
	if (!m_effect->initSkyBoxEffect(m_d3dDevice, L"CubeMapping.fxo"))
		return false;
	
	if (!BuildShaderResourceView())
		return false;

	return true;
}

bool CubeMapping::BuildBuffers()
{
	GeoGen::CreateSphere(100.f, 30, 30, m_skySphere);

	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = sizeof(Vertex) * m_skySphere.vertices.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;

	std::vector<Vertex> vertices(m_skySphere.vertices.size());
	for (UINT i = 0; i<m_skySphere.vertices.size(); ++i)
	{
		vertices[i].pos = m_skySphere.vertices[i].pos;
	}
	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = &vertices[0];
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&desc, &vData, &m_VB)))
	{
		MessageBox(NULL, L"Create Vertex Buffer failed!", L"Error", MB_OK);
		return false;
	}

	D3D11_BUFFER_DESC iDesc = { 0 };
	iDesc.ByteWidth = sizeof(UINT) * m_skySphere.indices.size();
	iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iDesc.Usage = D3D11_USAGE_DEFAULT;
	std::vector<UINT> indices(m_skySphere.indices.size());
	for (UINT i = 0; i<m_skySphere.indices.size(); ++i)
	{
		indices[i] = m_skySphere.indices[i];
	}
	D3D11_SUBRESOURCE_DATA iData;
	iData.pSysMem = &indices[0];
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&iDesc, &iData, &m_IB)))
	{
		MessageBox(NULL, L"Create Index Buffer failed!", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool CubeMapping::BuildShaderResourceView()
{
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"Texture//snowcube1024.dds", NULL, &m_SRVSkyBox)))
	{
		MessageBox(NULL, L"missing snowcube.dds", L"Error", MB_OK);
		return false;
	}
	return true;
}

bool CubeMapping::Update(float delta)
{
	
	m_camera->updateView();
	return true;
}
bool CubeMapping::Render()
{
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::Silver);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->IASetInputLayout(m_effect->getInputLayout());
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	ID3DX11EffectTechnique *tech = m_effect->m_skyBoxTech;
	

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT i = 0; i<techDesc.Passes; ++i)
	{
		XMFLOAT3 eyePos = m_camera->getPosition();
		//XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
		XMMATRIX T = XMMatrixIdentity();
		XMMATRIX WVP = XMMatrixMultiply(T, m_camera->viewProj());
		m_effect->setWorldViewProj(WVP);
		m_effect->setShaderResourceView(m_SRVSkyBox);
		tech->GetPassByIndex(i)->Apply(0, m_deviceContext);

		m_deviceContext->DrawIndexed(m_skySphere.indices.size(), 0, 0);
		m_deviceContext->RSSetState(0);
	}
	m_swapChain->Present(0, 0);

	return true;
}

void CubeMapping::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_lastPos.x = x;
	m_lastPos.y = y;
	SetCapture(m_hWnd);
}

void CubeMapping::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CubeMapping::OnMouseMove(WPARAM btnState, int x, int y)
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

void CubeMapping::OnKeyDown(WPARAM keyPressed) {}
void CubeMapping::OnKeyUp(WPARAM keyPressed) {}