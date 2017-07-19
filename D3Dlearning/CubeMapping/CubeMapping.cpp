#include "CubeMapping.h"

CubeMapping::CubeMapping(HINSTANCE hInst, wstring title, int width, int height) :
	WinApp(hInst, title, width, height),
	m_skyVB(nullptr), m_skyIB(nullptr),
	m_refVB(nullptr),m_refIB(nullptr),
	m_SRVSkyBox(nullptr)
{
	m_viewport.Width = static_cast<FLOAT>(m_clientWidth);
	m_viewport.Height = static_cast<FLOAT>(m_clientHeight);
	m_viewport.MaxDepth = 1.f;
	m_viewport.MinDepth = 0.f;
	m_viewport.TopLeftX = 0.f;
	m_viewport.TopLeftY = 0.f;
	for (int i = 0; i < 6; ++i)
		m_dynamicRTV[i] = nullptr;
	 m_dynamicDSV=nullptr;
	 m_dynamicSRV = nullptr;
	

	m_skyEffect = new SkyBoxEffect();
	m_basicEffect = new BasicEffect();
	m_camera = new Camera(width, height);
	//点光源
	m_pointLight.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f);
	m_pointLight.att = XMFLOAT3(0.005f, 0.0125f, 0.00125f);
	m_pointLight.pos = XMFLOAT3(10.f, 10.f, 10.f);
	m_pointLight.range = 50;
	//反射球材质
	m_materialRef.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f);
	m_materialRef.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_materialRef.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 30.f);
	//
	m_materialObj.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_materialObj.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	m_materialObj.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 100.0f);
	//世界变换矩阵
	XMMATRIX worldObj = XMMatrixTranslation(0.f, 0.f, -20.f);
	XMStoreFloat4x4(&m_objWorld, worldObj);
	XMMATRIX worldRef = XMMatrixTranslation(0.f, 0.f, 10.f);
	XMStoreFloat4x4(&m_refWorld,worldRef );
}
CubeMapping::~CubeMapping()
{
	if (m_camera != nullptr)
		delete m_camera;
	if (m_skyEffect != nullptr)
		delete m_skyEffect;
	if (m_basicEffect != nullptr)
		delete m_basicEffect;
	for (int i = 0; i < 6; ++i)
		SafeRelease(m_dynamicRTV[i]);
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
	if (!BuildDynamicCubeMappingViews())
		return false;
	if (!updateDynamicCameras(0,0,0))
		return false;
	
	return true;
}
bool CubeMapping::updateDynamicCameras(float x,float y,float z)
{
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.f, 1.f, 0.f),
		XMFLOAT3(0.f, 1.f, 0.f),
		XMFLOAT3(0.f, 0.f,-1.f),
		XMFLOAT3(0.f, 0.f, 1.f),
		XMFLOAT3(0.f, 1.f, 0.f),
		XMFLOAT3(0.f, 1.f, 0.f)
	};

	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x+1.f, y, z),
		XMFLOAT3(x-1.f,y, z),
		XMFLOAT3(x, y+1.f, z),
		XMFLOAT3(x,y-1.f,z),
		XMFLOAT3(x, y, z+1.f),
		XMFLOAT3(x, y,z-1.f)
	};

	for (UINT i = 0; i<6; ++i)
	{
		m_dynamicCameras[i].LookAt(XMFLOAT3(x, y, z), targets[i], ups[i]);
		m_dynamicCameras[i].setLens(XM_PI*0.5f, 1.f, 1.f, 1000.f);
		m_dynamicCameras[i].updateView();
	}
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
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"Texture//ice.dds", NULL, &m_SRVObjectSphere)))
	{
		MessageBox(NULL, L"missing ice.dds", L"Error", MB_OK);
		return false;
	}
	return true;
}
bool CubeMapping::BuildDynamicCubeMappingViews()
{
	D3D11_TEXTURE2D_DESC cubeMapDesc;
	cubeMapDesc.Width = m_cubeMapWidth;
	cubeMapDesc.Height = m_cubeMapHeight;
	cubeMapDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	cubeMapDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	cubeMapDesc.ArraySize = 6;
	cubeMapDesc.Usage = D3D11_USAGE_DEFAULT;
	cubeMapDesc.CPUAccessFlags = 0;
	cubeMapDesc.MipLevels = 0;
	cubeMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	cubeMapDesc.SampleDesc.Count = 1;
	cubeMapDesc.SampleDesc.Quality = 0;

	ID3D11Texture2D *cubeMap(nullptr);
	if (FAILED(m_d3dDevice->CreateTexture2D(&cubeMapDesc, 0, &cubeMap)))
	{
		MessageBox(NULL, L"Create dynamic Cube Map failed!", L"Error", MB_OK);
		return false;
	}
	D3D11_RENDER_TARGET_VIEW_DESC  rtvDesc;
	rtvDesc.Format = cubeMapDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;
	
	for (int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		if (FAILED(m_d3dDevice->CreateRenderTargetView(cubeMap, &rtvDesc, &m_dynamicRTV[i])))
		{
			SafeRelease(cubeMap);
			MessageBox(NULL, L"Create dynamic Cube Map  rtv failed!", L"Error", MB_OK);
			return false;
		}
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = cubeMapDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.MostDetailedMip = 0;
	if (FAILED(m_d3dDevice->CreateShaderResourceView(cubeMap, &srvDesc, &m_dynamicSRV)))
	{
		SafeRelease(cubeMap);
		MessageBox(NULL, L"Create dynamic Cube Map srv failed!", L"Error", MB_OK);
		return false;
	}
	SafeRelease(cubeMap);

	D3D11_TEXTURE2D_DESC dsDesc;
	dsDesc.Width = m_cubeMapWidth;
	dsDesc.Height = m_cubeMapHeight;
	dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsDesc.ArraySize = 1;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.MiscFlags = 0;
	dsDesc.MipLevels = 1;

	ID3D11Texture2D *depthStencilBuffer(nullptr);
	if (FAILED(m_d3dDevice->CreateTexture2D(&dsDesc, 0, &depthStencilBuffer)))
	{
		SafeRelease(depthStencilBuffer);
		MessageBox(NULL, L"Create dynamic depth stencil buffer failed!", L"Error", MB_OK);
		return false;
	}
	if (FAILED(m_d3dDevice->CreateDepthStencilView(depthStencilBuffer, 0, &m_dynamicDSV)))
	{
		SafeRelease(depthStencilBuffer);
		MessageBox(NULL, L"Create dynamic DSV failed!", L"Error", MB_OK);
		return false;
	}
	SafeRelease(depthStencilBuffer);

	m_dynamicViewport.Width = static_cast<float>(m_cubeMapWidth);
	m_dynamicViewport.Height = static_cast<float>(m_cubeMapHeight);
	m_dynamicViewport.TopLeftX = 0.f;
	m_dynamicViewport.TopLeftY = 0.f;
	m_dynamicViewport.MinDepth = 0.f;
	m_dynamicViewport.MaxDepth = 1.f;

	return true;
}
bool CubeMapping::Update(float delta)
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
	m_basicEffect->setPointLight(m_pointLight);
	XMFLOAT3 cameraPos = m_camera->getPosition();
	updateDynamicCameras(cameraPos.x, cameraPos.y, cameraPos.z);
	XMMATRIX worldObj = XMLoadFloat4x4(&m_objWorld)*XMMatrixRotationY(delta*XM_PI*0.124f);
	XMStoreFloat4x4(&m_objWorld, worldObj);
	return true;
}
bool CubeMapping::Render()
{
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Camera *tempCamera(nullptr);
	ID3D11RenderTargetView *rtv = nullptr;
	m_deviceContext->RSSetViewports(1, &m_dynamicViewport);
	
	for (int i = 0; i < 6; ++i)
	{
		rtv = m_dynamicRTV[i];
		m_deviceContext->OMSetRenderTargets(1, &rtv, m_dynamicDSV);
		m_deviceContext->ClearRenderTargetView(rtv, Colors::White);
		m_deviceContext->ClearDepthStencilView(m_dynamicDSV, D3D11_CLEAR_DEPTH, 1.f, 0);
		m_deviceContext->IASetInputLayout(m_skyEffect->getInputLayout());
		UINT stride = sizeof(PosVertex);
		UINT offset = 0;
		m_deviceContext->IASetVertexBuffers(0, 1, &m_skyVB, &stride, &offset);
		m_deviceContext->IASetIndexBuffer(m_skyIB, DXGI_FORMAT_R32_UINT, 0);
		tempCamera = &m_dynamicCameras[i];

		ID3DX11EffectTechnique *tech = m_skyEffect->m_skyBoxTech;
		D3DX11_TECHNIQUE_DESC techDesc;
		tech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			XMFLOAT3 pos = tempCamera->getPosition();
			XMMATRIX worldTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);
			XMMATRIX wvp = worldTrans*tempCamera->viewProj();
			m_skyEffect->setWorldViewProj(wvp);
			m_skyEffect->setShaderResourceView(m_SRVSkyBox);
			tech->GetPassByIndex(p)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_skySphere.indices.size(), 0, 0);
			m_deviceContext->RSSetState(0);
		}
		m_deviceContext->IASetInputLayout(m_basicEffect->getInputLayout());
		stride = sizeof(basic32Vertex);
		offset = 0;
		m_deviceContext->IASetVertexBuffers(0, 1, &m_refVB, &stride, &offset);
		m_deviceContext->IASetIndexBuffer(m_refIB, DXGI_FORMAT_R32_UINT, 0);
		tech = m_basicEffect->m_techTexLight;
		tech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			XMMATRIX world = XMLoadFloat4x4(&m_objWorld);
			XMMATRIX worldInvTrans = InverseTranspose(world);
			XMMATRIX wvp = world*tempCamera->viewProj();
			m_basicEffect->setWorldMatrix(world);
			m_basicEffect->setWorldInvTranspose(worldInvTrans);
			m_basicEffect->setWorldViewProj(wvp);
			m_basicEffect->setTexTrans(XMMatrixIdentity());
			m_basicEffect->setShaderResourceView(m_SRVObjectSphere);
			m_basicEffect->setMaterial(m_materialObj);
			tech->GetPassByIndex(p)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_refSphere.indices.size(), 0, 0);
		}
	}
	m_deviceContext->GenerateMips(m_dynamicSRV);
	//渲染反射球面
	m_deviceContext->RSSetViewports(1, &m_viewport);
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	if(usewfRender)
		m_deviceContext->RSSetState(RenderStates::WireFrameRS);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::Silver);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->IASetInputLayout(m_basicEffect->getInputLayout());
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(basic32Vertex);
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_refVB, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_refIB, DXGI_FORMAT_R32_UINT, 0);
	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique *tech = m_basicEffect->m_techNoTex;
	if (userefRender)
		tech = m_basicEffect->m_techCubeTexLight;
	tech->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		XMMATRIX worldRef = XMLoadFloat4x4(&m_refWorld);
		m_basicEffect->setWorldMatrix(worldRef);
		m_basicEffect->setWorldViewProj(worldRef*m_camera->viewProj());
		m_basicEffect->setWorldInvTranspose(InverseTranspose(worldRef));
		m_basicEffect->setMaterial(m_materialRef);
		m_basicEffect->setCubeShaderResourceView(m_dynamicSRV);
		tech->GetPassByIndex(i)->Apply(0, m_deviceContext);
		m_deviceContext->DrawIndexed(m_refSphere.indices.size(), 0, 0);
	}
	//天空盒
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
		m_skyEffect->setShaderResourceView(m_dynamicSRV);
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

void CubeMapping::OnKeyDown(WPARAM keyPressed) {
	switch (keyPressed)
	{
	case '1':
		usewfRender = !usewfRender;
		return;
	case '2':
		userefRender = !userefRender;
	default:
		break;
	}
}
void CubeMapping::OnKeyUp(WPARAM keyPressed) {}