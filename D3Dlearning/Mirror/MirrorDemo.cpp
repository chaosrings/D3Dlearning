#include "MirrorDemo.h"

MirrorDemo::MirrorDemo(HINSTANCE hInst, std::wstring title, int width, int height):WinApp(hInst, title, width, height),
	m_VBPlane(nullptr), m_IBPlane(nullptr),
	m_VBBox(nullptr), m_IBBox(nullptr),

	m_SRVWall(nullptr), m_SRVFloor(nullptr),
	m_SRVBox(nullptr), m_SRVMirror(nullptr),
	m_theta(1.5f*XM_PI),
	m_phy(XM_PI*0.45f),
	m_radius(8.f),
	techSelected(2)
{
	m_effect = new BlendEffect();
	m_pointLight.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f);
	m_pointLight.att = XMFLOAT3(0.01f, 0.025f, 0.025f);
	m_pointLight.pos = XMFLOAT3(2.f, 6.f, 2.f);
	m_pointLight.range = 50;
	
	//材质
	m_materialWall.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialWall.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	m_materialWall.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);

	m_materialBox.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialBox.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	m_materialBox.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);

	m_materialFloor.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialFloor.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.f);
	m_materialFloor.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	m_materialMirror.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialMirror.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	m_materialMirror.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 100.0f);

	m_materialShadow.ambient = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	m_materialShadow.diffuse = XMFLOAT4(0.f, 0.f, 0.f, 0.5f);
	m_materialShadow.specular = XMFLOAT4(0.f, 0.f, 0.f, 16.f);


	XMStoreFloat4x4(&m_worldWall, XMMatrixIdentity());
	XMStoreFloat4x4(&m_worldInvTransposeWall, XMMatrixIdentity());
	XMStoreFloat4x4(&m_texTransWall, XMMatrixIdentity());
	
	XMStoreFloat4x4(&m_worldFloor, XMMatrixIdentity());
	XMStoreFloat4x4(&m_worldInvTransposeFloor, XMMatrixIdentity());
	XMStoreFloat4x4(&m_texTransFloor, XMMatrixScaling(3.f, 2.f, 1.f));
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_worldBox, XMMatrixIdentity());
	XMStoreFloat4x4(&m_texTransBox, InverseTranspose(I));

	XMStoreFloat4x4(&m_worldMirror, XMMatrixIdentity());
	XMStoreFloat4x4(&m_worldInvTransposeMirror, XMMatrixIdentity());
	XMStoreFloat4x4(&m_texTransMirror, XMMatrixIdentity());

	XMStoreFloat4(&m_fogColor, Colors::Silver);
	m_fogStart = 1.4f;
	m_fogRange = 20.f;
	dx = 0;
	dz = 0;
}

MirrorDemo::~MirrorDemo()
{
	SafeRelease(m_SRVBox);
	SafeRelease(m_SRVFloor);
	SafeRelease(m_SRVWall);
	SafeRelease(m_SRVMirror);
	SafeRelease(m_VBPlane);
	SafeRelease(m_IBPlane);
	SafeRelease(m_VBBox);
	SafeRelease(m_IBBox);
	
	//if (m_effect != NULL)
		//delete m_effect;
	RenderStates::Release();
}


bool MirrorDemo::BuildBuffers()
{
	Vertex vertices[12];						//墙与地面的顶点

	vertices[0].pos = XMFLOAT3(-7.5f, -2.5f, 5.f);
	vertices[0].normal = XMFLOAT3(0.f, 0.f, -1.f);
	vertices[0].tex = XMFLOAT2(0.f, 1.f);

	vertices[1].pos = XMFLOAT3(-7.5f, 2.5f, 5.f);
	vertices[1].normal = XMFLOAT3(0.f, 0.f, -1.f);
	vertices[1].tex = XMFLOAT2(0.f, 0.f);

	vertices[2].pos = XMFLOAT3(-2.5f, 2.5f, 5.f);
	vertices[2].normal = XMFLOAT3(0.f, 0.f, -1.f);
	vertices[2].tex = XMFLOAT2(1.f, 0.f);

	vertices[3].pos = XMFLOAT3(-2.5f, -2.5f, 5.f);
	vertices[3].normal = XMFLOAT3(0.f, 0.f, -1.f);
	vertices[3].tex = XMFLOAT2(1.f, 1.f);

	vertices[4].pos = XMFLOAT3(2.5f, -2.5f, 5.f);
	vertices[4].normal = XMFLOAT3(0.f, 0.f, -1.f);
	vertices[4].tex = XMFLOAT2(2.f, 1.f);

	vertices[5].pos = XMFLOAT3(2.5f, 2.5f, 5.f);
	vertices[5].normal = XMFLOAT3(0.f, 0.f, -1.f);
	vertices[5].tex = XMFLOAT2(2.f, 0.f);

	vertices[6].pos = XMFLOAT3(7.5f, 2.5f, 5.f);
	vertices[6].normal = XMFLOAT3(0.f, 0.f, -1.f);
	vertices[6].tex = XMFLOAT2(3.f, 0.f);

	vertices[7].pos = XMFLOAT3(7.5f, -2.5f, 5.f);
	vertices[7].normal = XMFLOAT3(0.f, 0.f, -1.f);
	vertices[7].tex = XMFLOAT2(3.f, 1.f);

	vertices[8].pos = XMFLOAT3(-7.5f, -2.5f, -5.f);
	vertices[8].normal = XMFLOAT3(0.f, 1.f, 0.f);
	vertices[8].tex = XMFLOAT2(0.f, 1.f);

	vertices[9].pos = XMFLOAT3(-7.5f, -2.5f, 5.f);
	vertices[9].normal = XMFLOAT3(0.f, 1.f, 0.f);
	vertices[9].tex = XMFLOAT2(0.f, 0.f);

	vertices[10].pos = XMFLOAT3(7.5f, -2.5f, 5.f);
	vertices[10].normal = XMFLOAT3(0.f, 1.f, 0.f);
	vertices[10].tex = XMFLOAT2(1.f, 0.f);

	vertices[11].pos = XMFLOAT3(7.5f, -2.5f, -5.f);
	vertices[11].normal = XMFLOAT3(0.f, 1.f, 0.f);
	vertices[11].tex = XMFLOAT2(1.f, 1.f);

	D3D11_BUFFER_DESC vbPlaneDesc = { 0 };
	vbPlaneDesc.ByteWidth = sizeof(Vertex) * 12;
	vbPlaneDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbPlaneDesc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA planeData;
	planeData.pSysMem = vertices;
	planeData.SysMemPitch = 0;
	planeData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&vbPlaneDesc, &planeData, &m_VBPlane)))
		return false;


	UINT indices[24] =
	{
		0,1,2,	0,2,3,
		3,2,5,	3,5,4,
		4,5,6,	4,6,7,
		8,9,10,	8,10,11
	};
	D3D11_BUFFER_DESC ibPlaneDesc = { 0 };
	ibPlaneDesc.ByteWidth = 24 * sizeof(UINT);
	ibPlaneDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibPlaneDesc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA planeIData;
	planeIData.pSysMem = indices;
	planeIData.SysMemPitch = 0;
	planeIData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&ibPlaneDesc, &planeIData, &m_IBPlane)))
		return false;


	GeoGen::CreateBox(2.f, 2.f, 2.f, m_box);

	vector<Vertex>  boxVertices(m_box.vertices.size());
	for (UINT i = 0; i < m_box.vertices.size(); ++i)
	{
		boxVertices[i].pos = m_box.vertices[i].pos;
		boxVertices[i].normal = m_box.vertices[i].normal;
		boxVertices[i].tex = m_box.vertices[i].tex;
	}
	D3D11_BUFFER_DESC vbBoxDesc = { 0 };
	vbBoxDesc.ByteWidth = m_box.vertices.size()*sizeof(Vertex);
	vbBoxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbBoxDesc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA vbBoxData;
	vbBoxData.pSysMem = &boxVertices[0];
	vbBoxData.SysMemPitch = 0;
	vbBoxData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&vbBoxDesc, &vbBoxData, &m_VBBox)))
		return false;

	D3D11_BUFFER_DESC ibBoxDesc = { 0 };
	ibBoxDesc.ByteWidth = m_box.indices.size()*sizeof(UINT);
	ibBoxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibBoxDesc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA ibBoxData;
	ibBoxData.pSysMem = &m_box.indices[0];
	ibBoxData.SysMemPitch = 0;
	ibBoxData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&ibBoxDesc, &ibBoxData, &m_IBBox)))
		return false;

	return true;
}


bool MirrorDemo::BuildShaderReources()
{
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"Texture//brick.dds", NULL, &m_SRVWall)))
		return false;
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"Texture//Wood.dds", NULL, &m_SRVBox)))
		return false;
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"Texture//ice.dds",NULL, &m_SRVMirror)))
		return false;
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"Texture//checkboard.dds", NULL, &m_SRVFloor)))
		return false;
	return true;
}

bool MirrorDemo::Init()
{
	if (!WinApp::Init())
		return false;
	if (!m_effect->initBlendEffect(m_d3dDevice, L"FX//Basic.fxo"))
		return false;
	if (!RenderStates::InitRenderStates(m_d3dDevice))
		return false;
	if (!BuildBuffers())
		return false;
	if (!BuildShaderReources())
		return false;

	return true;
}
bool MirrorDemo::Update(float delta)
{
	XMMATRIX worldWall = XMLoadFloat4x4(&m_worldWall);
	XMMATRIX worldFloor = XMLoadFloat4x4(&m_worldFloor);
	XMMATRIX worldMirror = XMLoadFloat4x4(&m_worldMirror);
	XMMATRIX worldBox = XMLoadFloat4x4(&m_worldBox);
	worldBox = worldBox*XMMatrixTranslation(dx*0.02f, 0.f, dz*0.02f)*XMMatrixRotationZ(delta*0.2f*XM_PI)*XMMatrixRotationY(delta*0.2f*XM_PI);
	XMStoreFloat4x4(&m_worldBox, worldBox);
	XMStoreFloat4x4(&m_worldInvTransposeBox, InverseTranspose(worldBox));  //改变位置
	//更新世界矩阵，世界矩阵的逆矩阵的转置
	//计算视角矩阵
	XMVECTOR eyePos = XMVectorSet(m_radius*sin(m_phy)*cos(m_theta), m_radius*cos(m_phy), m_radius*sin(m_phy)*sin(m_theta), 1.f);
	XMVECTOR lookAt = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMMATRIX view = XMMatrixLookAtLH(eyePos, lookAt, up);
	//计算投影矩阵
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PI*0.25f, 1.f*m_clientWidth / m_clientHeight, 1.f, 1000.f);
	XMStoreFloat4x4(&m_view, view);
	XMStoreFloat4x4(&m_proj, proj);


	XMStoreFloat4x4(&m_worldViewProjWall, worldWall*view*proj);
	XMStoreFloat4x4(&m_worldViewProjFloor, worldFloor*view*proj);
	XMStoreFloat4x4(&m_worldViewProjMirror, worldMirror*view*proj);
	XMStoreFloat4x4(&m_worldViewProjBox, worldBox*view*proj);

	XMFLOAT3 eye;
	XMStoreFloat3(&eye,eyePos);
	m_effect->setEyePos(eye);
	m_effect->setPointLight(m_pointLight);
	m_effect->setFogColor(m_fogColor);
	m_effect->setFogStart(m_fogStart);
	m_effect->setFogRange(m_fogRange);
	return true;
}


bool MirrorDemo::Render()
{
	//清空基本法
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::Silver);
	m_deviceContext->IASetInputLayout(m_effect->getInputLayout());
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT offset = 0;
	UINT stride = sizeof(Vertex);
	ID3DX11EffectTechnique *tech = nullptr;
	switch (techSelected)
	{
	case 0:
		tech = m_effect->m_techNoLight;
		break;
	case 1:
		tech = m_effect->m_techNoTex;
		break;
	case 2:
		tech = m_effect->m_techTexLight;
		break;
	case 3:
		tech = m_effect->m_techTexLightFog;
		break;
	default:
		break;
	}
	
	m_deviceContext->IASetVertexBuffers(0, 1, &m_VBBox, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IBBox, DXGI_FORMAT_R32_UINT, 0);
	//更新相关数据
	

	//绘制箱子
	m_effect->setWorldMatrix(XMLoadFloat4x4(&m_worldBox));
	m_effect->setWorldInvTranspose(XMLoadFloat4x4(&m_worldInvTransposeBox));
	m_effect->setWorldViewProj(XMLoadFloat4x4(&m_worldViewProjBox));
	m_effect->setTexTrans(XMLoadFloat4x4(&m_texTransBox));
	m_effect->setMaterial(m_materialBox);
	m_effect->setShaderResourceView(m_SRVBox);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(m_box.indices.size(), 0,0);

	
	//绘制地面，墙面
	m_deviceContext->IASetVertexBuffers(0, 1, &m_VBPlane, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IBPlane, DXGI_FORMAT_R32_UINT, 0);

	m_effect->setWorldMatrix(XMLoadFloat4x4(&m_worldFloor));
	m_effect->setWorldInvTranspose(XMLoadFloat4x4(&m_worldInvTransposeFloor));
	m_effect->setWorldViewProj(XMLoadFloat4x4(&m_worldViewProjFloor));
	m_effect->setTexTrans(XMLoadFloat4x4(&m_texTransFloor));
	m_effect->setMaterial(m_materialFloor);
	m_effect->setShaderResourceView(m_SRVFloor);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(6, 18, 0);
	//墙面
	m_effect->setWorldMatrix(XMLoadFloat4x4(&m_worldWall));
	m_effect->setWorldInvTranspose(XMLoadFloat4x4(&m_worldInvTransposeWall));
	m_effect->setWorldViewProj(XMLoadFloat4x4(&m_worldViewProjWall));
	m_effect->setTexTrans(XMLoadFloat4x4(&m_texTransWall));
	m_effect->setMaterial(m_materialWall);
	m_effect->setShaderResourceView(m_SRVWall);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(6,0, 0);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(6, 12, 0);
	

	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_STENCIL, 1.f, 0);
	float BlendFactor[4] = { 0.f,0.f,0.f,0.f };
	m_deviceContext->OMSetBlendState(RenderStates::NoColorWriteBS,BlendFactor,0xffffffff);  
	m_deviceContext->OMSetDepthStencilState(RenderStates::MarkMirrorDSS, 0x1);
	m_effect->setWorldMatrix(XMLoadFloat4x4(&m_worldMirror));
	m_effect->setWorldInvTranspose(XMLoadFloat4x4(&m_worldInvTransposeMirror));
	m_effect->setWorldViewProj(XMLoadFloat4x4(&m_worldViewProjMirror));
	m_effect->setTexTrans(XMLoadFloat4x4(&m_texTransMirror));
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(6, 6, 0);

	m_deviceContext->OMSetBlendState(0, BlendFactor, 0xfffffff);
	m_deviceContext->OMSetDepthStencilState(NULL, 0x1);

	//绘制镜子中的箱子
	XMVECTOR refPlane = XMVectorSet(0.f, 0.f, -1.f, 5.f);
	XMMATRIX R = XMMatrixReflect(refPlane);

	XMMATRIX view = XMLoadFloat4x4(&m_view);
	XMMATRIX proj = XMLoadFloat4x4(&m_proj);
	XMMATRIX worldBox = XMLoadFloat4x4(&m_worldBox)*R;
	XMMATRIX worldInvTranspose = InverseTranspose(worldBox);
	XMMATRIX wvp = worldBox * view * proj;
	XMMATRIX texTrans = XMMatrixIdentity();

	m_deviceContext->IASetVertexBuffers(0, 1, &m_VBBox, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IBBox, DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->RSSetState(RenderStates::CounterClockFrontRS);
	m_deviceContext->OMSetDepthStencilState(RenderStates::DrawReflectionDSS, 0x1);
	
	m_effect->setWorldMatrix(worldBox);
	m_effect->setWorldViewProj(wvp);
	m_effect->setWorldInvTranspose(worldInvTranspose);
	m_effect->setTexTrans(texTrans);
	m_effect->setShaderResourceView(m_SRVBox);
	m_effect->setMaterial(m_materialBox);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(m_box.indices.size(), 0, 0);

	m_deviceContext->RSSetState(NULL);
	m_deviceContext->OMSetDepthStencilState(NULL, 0x1);
	

	m_deviceContext->OMSetBlendState(RenderStates::TransparentBS, BlendFactor, 0xffffff);
	m_deviceContext->IASetVertexBuffers(0, 1, &m_VBPlane, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IBPlane, DXGI_FORMAT_R32_UINT, 0);
	m_effect->setWorldMatrix(XMLoadFloat4x4(&m_worldMirror));
	m_effect->setWorldInvTranspose(XMLoadFloat4x4(&m_worldInvTransposeMirror));
	m_effect->setWorldViewProj(XMLoadFloat4x4(&m_worldViewProjMirror));
	m_effect->setTexTrans(XMLoadFloat4x4(&m_texTransMirror));
	m_effect->setMaterial(m_materialMirror);
	m_effect->setShaderResourceView(m_SRVMirror);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(6, 6, 0);

	//绘制箱子的影子
	m_deviceContext->IASetVertexBuffers(0, 1, &m_VBBox, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IBBox, DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->OMSetDepthStencilState(RenderStates::NoDoubleBlendDDS, 0x0);
	XMVECTOR grond = XMVectorSet(0.f, 1.f, 0.f, 2.5f);
	XMVECTOR lightDir = XMLoadFloat3(&m_pointLight.pos);
	XMMATRIX shadowMat = XMMatrixShadow(grond, lightDir);
	XMMATRIX worldShadow = XMLoadFloat4x4(&m_worldBox)*shadowMat*XMMatrixTranslation(0.f, 0.001f, 0.f);
	XMMATRIX worldViewProjShadow = worldShadow*view*proj;
	XMMATRIX worldInvTransposeShadow = InverseTranspose(worldShadow);
	XMMATRIX texTransShadow = XMMatrixIdentity();
	m_effect->setWorldMatrix(worldShadow);
	m_effect->setWorldViewProj(worldViewProjShadow);
	m_effect->setWorldInvTranspose(worldInvTransposeShadow);
	m_effect->setMaterial(m_materialShadow);
	m_effect->setTexTrans(texTransShadow);
	tech->GetPassByIndex(0)->Apply(0, m_deviceContext);
	m_deviceContext->DrawIndexed(m_box.indices.size(), 0, 0);
	m_deviceContext->OMSetDepthStencilState(NULL, 0);
	m_deviceContext->OMSetBlendState(NULL, BlendFactor, 0xfffffff);
	m_swapChain->Present(0, 0);
	return true;
}


void MirrorDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_lastPos.x = x;
	m_lastPos.y = y;

	SetCapture(m_hWnd);
}

void  MirrorDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void  MirrorDemo::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f*(x - m_lastPos.x));
		float dy = XMConvertToRadians(0.25f*(y - m_lastPos.y));

		m_theta -= dx;
		m_phy -= dy;

		m_phy = Clamp(0.01f, XM_PI - 0.01f, m_phy);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		float dRadius = 0.01f * static_cast<float>(x - m_lastPos.x);
		m_radius -= dRadius;

		m_radius = Clamp(3.f, 300.f, m_radius);
	}

	m_lastPos.x = x;
	m_lastPos.y = y;
}
void MirrorDemo::OnKeyDown(WPARAM keyPressed) {
	switch (keyPressed)
	{
	case '1':
		techSelected = 0;
		return;
	case '2':
		techSelected = 1;
		return;
	case '3':
		techSelected = 2;
		return;
	case '4':
		techSelected = 3;
		return;
	case 'A':
		dx = -1;
		dz = 0;
		return;
	case 'D':
		dx = 1;
		dz = 0;
		return;
	case 'W':
		dx = 0;
		dz = 1;
		return;
	case 'S':
		dx = 0;
		dz = -1;
		return;
		return;
	}
};
void MirrorDemo::OnKeyUp(WPARAM keyPressed) {
	switch (keyPressed)
	{
	case 65:
	case 68:
	case 87:
	case 83:
		dx = 0;
		dz = 0;
		return;
	case VK_SPACE:
		return;
	default:
		break;
	}
};