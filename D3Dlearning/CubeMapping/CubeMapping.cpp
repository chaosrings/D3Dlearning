#include "CubeMapping.h"

CubeMapping::CubeMapping(HINSTANCE hInst, wstring title, int width, int height) :
	WinApp(hInst, title, width, height),
	m_skyVB(nullptr), m_skyIB(nullptr),
	m_refVB(nullptr),m_refIB(nullptr),
	m_SRVSkyBox(nullptr)
{
	m_skyEffect = new SkyBoxEffect();
	m_basicEffect = new BasicEffect();
	m_camera = new Camera(width, height);
	//点光源
	m_pointLight.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f);
	m_pointLight.att = XMFLOAT3(0.005f, 0.0125f, 0.0125f);
	m_pointLight.pos = XMFLOAT3(10.f, 10.f, 10.f);
	m_pointLight.range = 50;
	//球材质
	m_materialRef.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f);
	m_materialRef.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_materialRef.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 30.f);

	//世界变换矩阵
	XMMATRIX worldRef = XMMatrixIdentity()*XMMatrixTranslation(0.f, 0.f, 10.f);
	XMStoreFloat4x4(&m_refWorld,worldRef );
	XMStoreFloat4x4(&m_refWorldInvTranspose, InverseTranspose(worldRef));
}
CubeMapping::~CubeMapping()
{
	if (m_camera != nullptr)
		delete m_camera;
	if (m_skyEffect != nullptr)
		delete m_skyEffect;
	if (m_basicEffect != nullptr)
		delete m_basicEffect;
	SafeRelease(m_skyVB);
	SafeRelease(m_skyIB);
	SafeRelease(m_refVB);
	SafeRelease(m_refIB);
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
	if (!m_skyEffect->initSkyBoxEffect(m_d3dDevice, L"CubeMapping.fxo"))
		return false;
	if (!m_basicEffect->initBasicEffect(m_d3dDevice, L"Basic.fxo"))
		return false;
	if (!BuildShaderResourceView())
		return false;
	m_camera->setPosition(0.f, 0.f, -4.f);
	return true;
}

bool CubeMapping::BuildBuffers()
{
	GeoGen::CreateSphere(100.f, 30, 30, m_skySphere);
	GeoGen::CreateSphere(4.f, 30, 30, m_refSphere);

	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = sizeof(PosVertex) * (m_skySphere.vertices.size());
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;

	std::vector<PosVertex> vertices(m_skySphere.vertices.size());
	for (UINT i = 0; i<m_skySphere.vertices.size(); ++i)
	{
		vertices[i].pos = m_skySphere.vertices[i].pos;
	}
	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = &vertices[0];
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&desc, &vData, &m_skyVB)))
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
	if (FAILED(m_d3dDevice->CreateBuffer(&iDesc, &iData, &m_skyIB)))
	{
		MessageBox(NULL, L"Create Index Buffer failed!", L"Error", MB_OK);
		return false;
	}


	D3D11_BUFFER_DESC refDesc = { 0 };
	refDesc.ByteWidth = sizeof(basic32Vertex) * (m_refSphere.vertices.size());
	refDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	refDesc.Usage = D3D11_USAGE_DEFAULT;

	std::vector<basic32Vertex> verticesRef(m_refSphere.vertices.size());
	for (UINT i = 0; i<m_refSphere.vertices.size(); ++i)
	{
		verticesRef[i].pos = m_refSphere.vertices[i].pos;
		verticesRef[i].normal = m_refSphere.vertices[i].normal;
		verticesRef[i].tex = m_refSphere.vertices[i].tex;
	}
	D3D11_SUBRESOURCE_DATA vRefData;
	vRefData.pSysMem = &verticesRef[0];
	vRefData.SysMemPitch = 0;
	vRefData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&refDesc, &vRefData, &m_refVB)))
	{
		MessageBox(NULL, L"Create Vertex Buffer failed!", L"Error", MB_OK);
		return false;
	}

	D3D11_BUFFER_DESC iRefDesc = { 0 };
	iRefDesc.ByteWidth = sizeof(UINT) * m_refSphere.indices.size();
	iRefDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iRefDesc.Usage = D3D11_USAGE_DEFAULT;
	std::vector<UINT> indicesRef(m_refSphere.indices.size());
	for (UINT i = 0; i<m_refSphere.indices.size(); ++i)
	{
		indicesRef[i] = m_refSphere.indices[i];
	}
	D3D11_SUBRESOURCE_DATA iRefData;
	iRefData.pSysMem = &indicesRef[0];
	iRefData.SysMemPitch = 0;
	iRefData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&iRefDesc, &iRefData, &m_refIB)))
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

	if (KeyDown('A'))
	{
		m_camera->Strafe(6.f*delta);
	}
	else if (KeyDown('D'))
	{
		m_camera->Strafe(-6.f*delta);
	}
	if (KeyDown('W'))
	{
		m_camera->Walk(-6.f*delta);
	}
	else if (KeyDown('S'))
	{
		m_camera->Walk(6.f*delta);
	}

	m_camera->updateView();
	m_basicEffect->setEyePos(m_camera->getPosition());
	m_basicEffect->setPointLight(m_pointLight);

	XMMATRIX worldRef = XMLoadFloat4x4(&m_refWorld);
	XMStoreFloat4x4(&m_refWorldInvTranspose, InverseTranspose(worldRef));
	XMStoreFloat4x4(&m_refWorldViewProj, worldRef*m_camera->viewProj());
	return true;
}
bool CubeMapping::Render()
{
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::Silver);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->IASetInputLayout(m_basicEffect->getInputLayout());
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(basic32Vertex);
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_refVB, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_refIB, DXGI_FORMAT_R32_UINT, 0);
	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique *tech= m_basicEffect->m_techCubeTexLight;
	tech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		m_basicEffect->setWorldMatrix(XMLoadFloat4x4(&m_refWorld));
		m_basicEffect->setWorldViewProj(XMLoadFloat4x4(&m_refWorldViewProj));
		m_basicEffect->setWorldInvTranspose(XMLoadFloat4x4(&m_refWorldInvTranspose));
		m_basicEffect->setMaterial(m_materialRef);
		m_basicEffect->setCubeShaderResourceView(m_SRVSkyBox);
		tech->GetPassByIndex(i)->Apply(0, m_deviceContext);
		m_deviceContext->DrawIndexed(m_refSphere.indices.size(), 0, 0);
	}

	stride = sizeof(PosVertex);
	offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_skyVB, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_skyIB, DXGI_FORMAT_R32_UINT, 0);
	tech = m_skyEffect->m_skyBoxTech;
	tech->GetDesc(&techDesc);
	for (UINT i = 0; i<techDesc.Passes; ++i)
	{
		XMFLOAT3 eyePos = m_camera->getPosition();
		XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
		XMMATRIX WVP = XMMatrixMultiply(T, m_camera->viewProj());
		m_skyEffect->setWorldViewProj(WVP);
		m_skyEffect->setShaderResourceView(m_SRVSkyBox);
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