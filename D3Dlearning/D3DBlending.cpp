#include "D3DBlending.h"

D3DBlending::D3DBlending(HINSTANCE hInst, wstring title, int width, int height)
	:WinApp(hInst, title, width, height), m_VB(nullptr), m_IB(nullptr),
	m_SRVBasin(nullptr), m_SRVBasinBottom(nullptr), m_SRVWater(nullptr), m_SRVBox(nullptr),
	m_theta(XM_PI*0.25f),
	m_phy(XM_PI * 0.45f),
	m_radius(8.f)
{
	m_effect = new BlendEffect();
	m_pointLight.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f);
	m_pointLight.att = XMFLOAT3(0.01f, 0.025f, 0.025f);
	m_pointLight.pos = XMFLOAT3(2.f, 6.f, 2.f);
	m_pointLight.range = 50;

	m_materialBasin.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialBasin.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	m_materialBasin.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);

	m_materialBox.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialBox.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	m_materialBox.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);

	m_materialWater.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialWater.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	m_materialWater.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_worldBasin, I);
	XMStoreFloat4x4(&m_worldInvTransposeBasin, InverseTranspose(I));
	XMStoreFloat4x4(&m_worldBasinBottom, I);
	XMStoreFloat4x4(&m_worldInvTransposeBasinBottom, InverseTranspose(I));
	
	I= XMMatrixTranslation(0.f, 0.1f, 0.f);
	XMStoreFloat4x4(&m_worldBox, I);
	XMStoreFloat4x4(&m_worldInvTransposeBox, InverseTranspose(I));
	I = XMMatrixTranslation(0.f, 0.f, 0.f);
	XMStoreFloat4x4(&m_worldWater, I);
	XMStoreFloat4x4(&m_worldInvTransposeWater, InverseTranspose(I));

	XMMATRIX texTransBasin = XMMatrixScaling(10.f, 1.f, 1.f);
	XMMATRIX texTransWater = XMMatrixScaling(5.f, 5.f, 1.f);
	//纹理坐标变换矩阵
	XMStoreFloat4x4(&m_texTransBasin, texTransBasin);
	XMStoreFloat4x4(&m_texTransBasinBottom,texTransBasin);
	XMStoreFloat4x4(&m_texTransWater, texTransWater);
	XMStoreFloat4x4(&m_texTransBox, XMMatrixIdentity());
}
D3DBlending::~D3DBlending()
{
	SafeRelease(m_VB);
	SafeRelease(m_IB);
	SafeRelease(m_SRVBasin);
	SafeRelease(m_SRVBasinBottom);
	SafeRelease(m_SRVWater);
	SafeRelease(m_SRVBox);
}
bool D3DBlending::BuildBuffers()
{
	GeoGen::CreateCylinder(10.f, 10.f, 4.f, 50, 10, m_basin);
	m_basinVStart = 0;
	m_basinIStart = 0;
	m_basinICount = m_basin.indices.size();

	m_basinBVStart = m_basin.vertices.size() + m_basinVStart;
	m_basinBIStart = m_basin.indices.size() + m_basinIStart;

	GeoGen::AddCylinderBottomCap(10.f, 10.f, 4.f, 50, 10, m_basin);
	m_basinBICount = m_basin.indices.size() - m_basinICount;

	GeoGen::CreateGrid(20.f, 20.f, 1, 1, m_water);
	m_waterVStart = m_basin.vertices.size();
	m_waterIStart = m_basin.indices.size();

	GeoGen::CreateBox(2.f, 2.f, 2.f, m_box);
	m_boxVStart = m_waterVStart + m_water.vertices.size();
	m_boxIStart = m_waterIStart + m_water.indices.size();

	UINT totalVerts = m_boxVStart + m_box.vertices.size();
	UINT totalIndices = m_boxIStart + m_box.indices.size();

	vector<Vertex> allVerts(totalVerts);

	for (UINT i = 0; i < m_basin.vertices.size(); ++i)
	{
		allVerts[i].pos = m_basin.vertices[i].pos;
		XMFLOAT3 normal = m_basin.vertices[i].normal;
		allVerts[i].normal = XMFLOAT3(-normal.x, -normal.y, -normal.z);
		allVerts[i].tex = m_basin.vertices[i].tex;
	}
	for (UINT i = 0; i < m_water.vertices.size(); ++i)
	{
		allVerts[i + m_waterVStart].pos = m_water.vertices[i].pos;
		allVerts[i + m_waterVStart].normal = m_water.vertices[i].normal;
		allVerts[i + m_waterVStart].tex = m_water.vertices[i].tex;
	}
	for (UINT i = 0; i < m_box.vertices.size(); ++i)
	{
		allVerts[i + m_boxVStart].pos = m_box.vertices[i].pos;
		allVerts[i + m_boxVStart].normal = m_box.vertices[i].normal;
		allVerts[i + m_boxVStart].tex = m_box.vertices[i].tex;
	}

	D3D11_BUFFER_DESC vbDesc = { 0 };
	vbDesc.ByteWidth = sizeof(Vertex)*totalVerts;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA vbData = { 0 };
	vbData.pSysMem = &allVerts[0];
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&vbDesc, &vbData, &m_VB)))
	{
		MessageBox(NULL, L"Create Vertex Buffer failed!", L"Error", MB_OK);
		return false;
	}

	vector<UINT>   allIndices(totalIndices);
	for (UINT i = 0; i < m_basin.indices.size(); ++i)
	{
		allIndices[i] = m_basin.indices[i];
	}
	for (UINT i = 0; i < m_water.indices.size(); ++i)
	{
		allIndices[i + m_waterIStart] = m_water.indices[i];
	}
	for (UINT i = 0; i < m_box.indices.size(); ++i)
	{
		allIndices[i + m_boxIStart] = m_box.indices[i];
	}
	D3D11_BUFFER_DESC ibDesc = { 0 };
	ibDesc.ByteWidth = sizeof(UINT)*totalIndices;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = &allIndices[0];
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&ibDesc, &ibData, &m_IB)))
	{
		MessageBox(NULL, L"Create Index Buffer failed!", L"Error", MB_OK);
		return false;
	}
	return true;
}
bool D3DBlending::BuildShaderResourceView()
{
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"checkboard.dds", &m_RBasin, &m_SRVBasin)))
	{

		MessageBox(NULL, L"Create SRV of basin failed!", L"Error", MB_OK);
		return false;
	}
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"checkboard.dds", &m_RBasinBottom, &m_SRVBasinBottom)))
	{
		MessageBox(NULL, L"Create SRV of basin bottom failed!", L"Error", MB_OK);
		return false;
	}
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"water.dds", &m_RWater, &m_SRVWater)))
	{
		MessageBox(NULL, L"Create SRV of water failed!", L"Error", MB_OK);
		return false;
	}
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"Wood.dds", &m_RBox, &m_SRVBox)))
	{
		MessageBox(NULL, L"Create SRV of box failed!", L"Error", MB_OK);
		return false;
	}
	return true;

}
bool D3DBlending::Init()
{
	if (!WinApp::Init())
		return false;
	if (!m_effect->initBlendEffect(m_d3dDevice, L"basicBlend.fxo"))
		return false;
	if (!BuildShaderResourceView())
		return false;
	if (!BuildBuffers())
		return false;
	
	return true;
}

bool D3DBlending::Update(float delta)
{
	XMMATRIX worldBasin = XMLoadFloat4x4(&m_worldBasin);
	XMMATRIX worldBasinB = XMLoadFloat4x4(&m_worldBasinBottom);
	XMMATRIX worldWater = XMLoadFloat4x4(&m_worldWater);
	XMMATRIX worldBox = XMLoadFloat4x4(&m_worldBox);

	XMVECTOR eyePos = XMVectorSet(m_radius*sin(m_phy)*cos(m_theta), m_radius*cos(m_phy), m_radius*sin(m_phy)*sin(m_theta), 1.f);
	XMVECTOR lookAt = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	XMMATRIX view = XMMatrixLookAtLH(eyePos, lookAt, up);
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PI*0.25f, 1.f*m_clientWidth / m_clientHeight, 1.f, 1000.f);

	XMStoreFloat4x4(&m_worldViewProjBasin, worldBasin*view*proj);
	XMStoreFloat4x4(&m_worldViewProjBasinBottom, worldBasinB*view*proj);
	XMStoreFloat4x4(&m_worldViewProjWater, worldWater*view*proj);
	XMStoreFloat4x4(&m_worldViewProjBox, worldBox*view*proj);

	m_effect->setPointLight(m_pointLight);

	XMFLOAT3 eye;
	XMStoreFloat3(&eye, eyePos);
	m_effect->setEyePos(eye);
	return true;
}
bool D3DBlending::Render()
{
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::Silver);
	m_deviceContext->IASetInputLayout(m_effect->getInputLayout());
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique *tech = m_effect->m_techTexLight;
	tech->GetDesc(&techDesc);

	ID3D11RasterizerState *CounterClockFrontRS;
	D3D11_RASTERIZER_DESC ccfDesc;
	ZeroMemory(&ccfDesc, sizeof(ccfDesc));
	ccfDesc.CullMode = D3D11_CULL_BACK;
	ccfDesc.FillMode = D3D11_FILL_SOLID;
	ccfDesc.FrontCounterClockwise = true;
	ccfDesc.DepthClipEnable = true;
	if (FAILED(m_d3dDevice->CreateRasterizerState(&ccfDesc, &CounterClockFrontRS)))
	{
		MessageBox(NULL, L"Create 'NoCull' rasterizer state failed!", L"Error", MB_OK);
		return false;
	}
	m_deviceContext->RSSetState(CounterClockFrontRS);

	m_effect->setWorldMatrix(XMLoadFloat4x4(&m_worldBasin));
	m_effect->setWorldViewProj(XMLoadFloat4x4(&m_worldViewProjBasin));
	m_effect->setWorldInvTranspose(XMLoadFloat4x4(&m_worldInvTransposeBasin));
	m_effect->setMaterial(m_materialBasin);
	m_effect->setTexTrans(XMLoadFloat4x4(&m_texTransBasin));
	m_effect->setShaderResourceView(m_SRVBasin);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(m_basin.indices.size(), m_basinIStart, m_basinVStart);


	//箱子
	m_deviceContext->RSSetState(0);
	m_effect->setWorldMatrix(XMLoadFloat4x4(&m_worldBox));
	m_effect->setWorldViewProj(XMLoadFloat4x4(&m_worldViewProjBox));
	m_effect->setWorldInvTranspose(XMLoadFloat4x4(&m_worldInvTransposeBox));
	m_effect->setMaterial(m_materialBox);
	m_effect->setTexTrans(XMLoadFloat4x4(&m_texTransBox));
	m_effect->setShaderResourceView(m_SRVBox);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(m_box.indices.size(), m_boxIStart, m_boxVStart);
	//透明效果
	D3D11_BLEND_DESC transDesc;
	//先创建一个混合状态的描述  
	transDesc.AlphaToCoverageEnable = false;        //关闭AlphaToCoverage  
	transDesc.IndependentBlendEnable = false;       //不针对多个RenderTarget使用不同的混合状态  
													//因此只设置第一个数组元素即可  
	transDesc.RenderTarget[0].BlendEnable = true;
	transDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; 
	ID3D11BlendState  *ptrans;
	m_d3dDevice->CreateBlendState(&transDesc,&ptrans);
	float blendFactor[4] = { 0.f,0.f,0.f,0.f };
	m_deviceContext->OMSetBlendState(ptrans, blendFactor, 0xffffff);
	//水
	m_effect->setWorldMatrix(XMLoadFloat4x4(&m_worldWater));
	m_effect->setWorldViewProj(XMLoadFloat4x4(&m_worldViewProjWater));
	m_effect->setWorldInvTranspose(XMLoadFloat4x4(&m_worldInvTransposeWater));
	m_effect->setMaterial(m_materialWater);
	m_effect->setTexTrans(XMLoadFloat4x4(&m_texTransWater));
	m_effect->setShaderResourceView(m_SRVWater);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(m_water.indices.size(), m_waterIStart, m_waterVStart);

	m_deviceContext->OMSetBlendState(0, blendFactor, 0xffffff);
	m_swapChain->Present(0, 0);
	return true;
}
void D3DBlending::OnMouseDown(WPARAM btnState, int x, int y)
{
	SetCapture(m_hWnd);
	m_lastPos.x = x;
	m_lastPos.y = y;
}

void D3DBlending::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void D3DBlending::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f*(x - m_lastPos.x));
		float dy = XMConvertToRadians(0.25f*(y - m_lastPos.y));

		m_theta -= dx;
		m_phy -= dy;

		m_phy = Clamp(0.01f, 0.5f*XM_PI, m_phy);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		float dRadius = 0.01f * static_cast<float>(x - m_lastPos.x);
		m_radius -= dRadius;

		m_radius = Clamp(3.f, 10.f, m_radius);
	}

	m_lastPos.x = x;
	m_lastPos.y = y;
}
void D3DBlending::OnKeyDown(WPARAM keyPressed) {}
void D3DBlending::OnKeyUp(WPARAM keyPressed) {}