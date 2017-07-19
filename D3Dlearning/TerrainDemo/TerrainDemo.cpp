#include "TerrainDemo.h"

TerrainDemo::TerrainDemo(HINSTANCE hInst, wstring title, int width, int height) :WinApp(hInst, title, width, height),
m_VB(nullptr), m_IB(nullptr),
m_SRVTerrain(nullptr)
{
	
	m_camera = new Camera(width, height);
	m_terrain = new Terrain(50.f, 50.f, 63, 63);
	m_basicEffect = new BasicEffect();
	m_camera->setPosition(0.f, 50.f, 0.f);
	XMStoreFloat4x4(&m_worldTerrain, XMMatrixTranslation(0.f, 0.f, 10.f));

	m_materialTerrain.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialTerrain.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	m_materialTerrain.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 100.0f);


	m_pointLight.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f);
	m_pointLight.att = XMFLOAT3(0.005f, 0.0125f, 0.00525f);
	m_pointLight.pos = XMFLOAT3(10.f, 10.f, 10.f);
	m_pointLight.range = 50;
}
TerrainDemo::~TerrainDemo()
{
	if (m_camera != nullptr)
		delete m_camera;
	if (m_basicEffect != nullptr)
		delete m_basicEffect;
	SafeRelease(m_IB);
	SafeRelease(m_VB);
	SafeRelease(m_SRVTerrain);
}

bool TerrainDemo::Init()
{
	if (!WinApp::Init())
		return false;
	if (!m_basicEffect->initBasicEffect(m_d3dDevice, L"Basic.fxo"))
		return false;
	if (!m_terrain->InitTerrain("TerrainDemo//mountain64.raw", 12.f))
		return false;
	if (!BuildBuffers())
		return false;
	if (!BuildSRV())
		return false;
	
	return true;
}

bool TerrainDemo::BuildBuffers()
{
	vector<Vertex> vertices(m_terrain->vertices.size());
	vector<UINT>   indices(m_terrain->indices.size());
	for (UINT i = 0; i < vertices.size(); ++i)
	{
		vertices[i].pos = m_terrain->vertices[i].pos;
		vertices[i].normal = m_terrain->vertices[i].normal;
		vertices[i].tex = m_terrain->vertices[i].tex;
	}
	for (UINT i = 0; i < indices.size(); ++i)
	{
		indices[i] = m_terrain->indices[i];
	}
	D3D11_BUFFER_DESC vbDesc = { 0 };
	vbDesc.ByteWidth = sizeof(Vertex)*vertices.size();
	vbDesc.CPUAccessFlags = 0;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vbData = { 0 };
	vbData.pSysMem = &vertices[0];
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	if (FAILED(m_d3dDevice->CreateBuffer(&vbDesc, &vbData, &m_VB)))
	{
		MessageBox(NULL, L"Build Vertex Buffer Failed", L"Error", MB_OK);
		return false;
	}
	D3D11_BUFFER_DESC ibDesc = { 0 };
	ibDesc.ByteWidth = sizeof(UINT)*indices.size();
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
	ibDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = { 0 };
	ibData.pSysMem = &indices[0];
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	if (FAILED(m_d3dDevice->CreateBuffer(&ibDesc, &ibData, &m_IB)))
	{
		MessageBox(NULL, L"Build Index Buffer Failed", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool TerrainDemo::BuildSRV()
{
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"Texture//ice.dds", NULL, &m_SRVTerrain)))
	{
		MessageBox(NULL, L"Build SRV Failed", L"Error", MB_OK);
		return false;
	}
	return true;
}
bool TerrainDemo::Update(float delta)
{
	if (KeyDown('A'))
	{
		m_camera->Strafe(-6.f*delta);
	}
	else if (KeyDown('D'))
	{
		m_camera->Strafe(6.f*delta);
	}
	if (KeyDown('W'))
	{
		m_camera->Walk(6.f*delta);
	}
	else if (KeyDown('S'))
	{
		m_camera->Walk(-6.f*delta);
	}
	m_camera->updateView();
	m_basicEffect->setEyePos(m_camera->getPosition());
	m_pointLight.pos = m_camera->getPosition();
	m_basicEffect->setPointLight(m_pointLight);
	return true;
}

bool TerrainDemo::Render()
{
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::Silver);
	m_deviceContext->IASetInputLayout(m_basicEffect->getInputLayout());
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	//m_deviceContext->IAGetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	//写代码10分钟debug一小时，大概就是这样
	m_deviceContext->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3DX11EffectTechnique *tech = m_basicEffect->m_techTexLight;
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		XMMATRIX worldTerrain = XMLoadFloat4x4(&m_worldTerrain);
		m_basicEffect->setWorldMatrix(worldTerrain);
		m_basicEffect->setWorldViewProj((worldTerrain*m_camera->viewProj()));
		m_basicEffect->setWorldInvTranspose(InverseTranspose(worldTerrain));
		m_basicEffect->setMaterial(m_materialTerrain);
		m_basicEffect->setShaderResourceView(m_SRVTerrain);
		//m_basicEffect->setTexTrans(XMMatrixIdentity());
		m_basicEffect->setTexTrans(XMMatrixScaling(0.02f, 0.02f, 0.02f));
		tech->GetPassByIndex(i)->Apply(0, m_deviceContext);
		m_deviceContext->DrawIndexed(m_terrain->indices.size(), 0, 0);
	}
	m_swapChain->Present(0, 0);
	return true;
}