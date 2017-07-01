#include "D3Dlight.h"

D3Dlight::D3Dlight(HINSTANCE hInst, std::wstring title, int width, int height) :
	WinApp(hInst, title, width, height), m_inputLayout(nullptr),
	m_textureView(nullptr),
	m_VertexBuffer(nullptr), m_IndexBuffer(nullptr), m_fx(nullptr),
	m_fxWorldViewProj(nullptr), m_fxWorld(nullptr), m_fxWorldInvTranspose(nullptr),
	m_fxMaterial(nullptr), m_fxdirLights(nullptr), m_numLights(1),
	m_fxEyePos(nullptr), m_theta(XM_PI*1.5f),
	m_phy(XM_PI * 0.4f),
	m_radius(20.f)
{

	XMMATRIX gridWorld = XMMatrixIdentity();
	XMStoreFloat4x4(&m_gridWorld, gridWorld);
	dx = 0; dz = 0;
	XMMATRIX sphereWorld = XMMatrixIdentity()*XMMatrixTranslation(0, 2.f, 0);
	XMStoreFloat4x4(&m_sphereWorld[0], sphereWorld);
	for (UINT i = 0; i < 2; i++)
	{
		for (UINT j = 0; j < 2; j++)
		{
			XMMATRIX setSphere = XMMatrixTranslation(-5.f + i*10.f, 2.f, -5.f + j*10.f);
			XMStoreFloat4x4(&m_sphereWorld[i * 2 + j + 1], setSphere);
		}
	}
	//点光源
	m_pointLight.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pointLight.specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f);
	m_pointLight.att = XMFLOAT3(0.02f, 0.05f, 0.05f);
	m_pointLight.pos = XMFLOAT3(0.f,6.f, 0.f);
	m_pointLight.range = 20;
	//材质
	m_matGrid.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	m_matGrid.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	m_matGrid.specular = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);


	m_matSphere.ambient = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
	m_matSphere.diffuse = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f);
	m_matSphere.specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 0.9f);

}
D3Dlight::~D3Dlight()
{
	SafeRelease(m_inputLayout);
	SafeRelease(m_VertexBuffer);
	SafeRelease(m_IndexBuffer);
	SafeRelease(m_fx);
}
bool D3Dlight::Init()
{
	if (!WinApp::Init())
		return false;
	if (!BuildFX())
		return false;
	if (!BuildTexture())
		return false;
	if (!BuildInputLayout())
		return false;
	if (!BuildBuffers())
		return false;

	return true;
}
bool D3Dlight::BuildTexture()
{
	if (FAILED(CreateDDSTextureFromFile(m_d3dDevice, L"Wood.dds", &m_TextureResource, &m_textureView))) //创建纹理和相应的视图
		return false;
	return true;
}
bool D3Dlight::Update(float delta)     //每一帧更新               
{
		m_fxPointLight->SetRawValue((void*)&m_pointLight, 0, sizeof(m_pointLight));
		m_fxTexture->SetResource(m_textureView);
		//接受键盘控制
		XMMATRIX world_sphere = XMLoadFloat4x4(&m_sphereWorld[0]);
		world_sphere = XMMatrixMultiply(world_sphere, XMMatrixTranslation(dx*0.02f, 0, dz*0.02f));
		XMStoreFloat4x4(&m_sphereWorld[0], world_sphere);
		if (isJumping)
		{
			world_sphere = XMMatrixMultiply(world_sphere, XMMatrixTranslation(0.f, v*delta, 0));
			XMStoreFloat4x4(&m_sphereWorld[0], world_sphere);
			v = v + delta*(-20.8f);
			if (abs(v) >= v0)
			{
				v = (-0.7f)*v;
				v0 = v;
				if (abs(v0) < 0.2f)
				{
					isJumping = false;
					v0 = 0.f;
					v = 0.f;
				}
			}
		}

		for (int i = 1; i <= 4; i++)
		{
			world_sphere = XMLoadFloat4x4(&m_sphereWorld[i]);
			world_sphere = world_sphere*XMMatrixRotationY(delta*XM_PI*0.125);
			XMStoreFloat4x4(&m_sphereWorld[i], world_sphere);
		}
		XMVECTOR pos = XMVectorSet(m_radius*sin(m_phy)*cos(m_theta), m_radius*cos(m_phy), m_radius*sin(m_phy)*sin(m_theta), 1.f);
		XMVECTOR target = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
		XMStoreFloat4x4(&m_view, view);
		//保存观察点
		XMStoreFloat3(&m_eyePos, pos);

		m_fxEyePos->SetRawValue((void*)&m_eyePos, 0, sizeof(m_eyePos));

		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PI*0.25f, 1.f*m_clientWidth / m_clientHeight, 1.f, 1000.f);
		XMStoreFloat4x4(&m_proj, proj);
	return true;
}
bool D3Dlight::Render()
{
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, Colors::Silver);
	m_deviceContext->IASetInputLayout(m_inputLayout);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX  view = XMLoadFloat4x4(&m_view);
	XMMATRIX  proj = XMLoadFloat4x4(&m_proj);

	ostringstream os;
	os << "Light" << m_numLights;
	string techName = os.str().c_str();
	ID3DX11EffectTechnique *tech = m_fx->GetTechniqueByName(techName.c_str());
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	
	XMMATRIX world = XMLoadFloat4x4(&m_gridWorld);
	
	m_fxWorld->SetMatrix(reinterpret_cast<float*>(&world));
	m_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*view*proj)));
	XMVECTOR det = XMMatrixDeterminant(world);
	XMMATRIX worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&det, world));
	//设置世界+反+转置矩阵
	m_fxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
	//设置材质为grid的材质
	m_fxMaterial->SetRawValue((void*)&m_matGrid, 0, sizeof(m_matGrid));
	
	//画出平面
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		tech->GetPassByIndex(i)->Apply(0, m_deviceContext);
		m_deviceContext->DrawIndexed(m_grid.indices.size(), m_gridIStart,m_gridVStart);
	}
	
	for (int i = 0; i <5; i++)
	{
		XMMATRIX world = XMLoadFloat4x4(&m_sphereWorld[i]);

		m_fxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		m_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*view*proj)));
		XMVECTOR det = XMMatrixDeterminant(world);
		XMMATRIX worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&det, world));
		//设置世界+反+转置矩阵
		m_fxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
		//设置材质为球的材质
		m_fxMaterial->SetRawValue((void*)(&m_matSphere), 0, sizeof(m_matSphere));
		for (UINT i = 0; i<techDesc.Passes; ++i)
		{
			tech->GetPassByIndex(i)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_sphere.indices.size(), m_sphereIStart, m_sphereVStart);
		}
	}

	m_swapChain->Present(0, 0);
	return true;
}
void D3Dlight::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_lastPos.x = x;
	m_lastPos.y = y;

	SetCapture(m_hWnd);
}

void D3Dlight::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void D3Dlight::OnMouseMove(WPARAM btnState, int x, int y)
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
void D3Dlight::OnKeyDown(WPARAM keyPressed) {
	switch (keyPressed)
	{
	case 49:
		m_numLights = 0;
		return;
	case 50:
		m_numLights = 1;
		return;
	case 51:
		m_numLights = 1;
		return;
	case 65:
		dx = -1;
		dz = 0;
		return;
	case 68:
		dx = 1;
		dz = 0;
		return;
	case 87:
		dx = 0;
		dz = 1;
		return;
	case 83:
		dx = 0;
		dz = -1;
		return;
	case VK_SPACE:
		if (!isJumping)
		{
			v = takeoffSpeed;
			v0 = v;
			isJumping = true;
		}
		return;
	}
};
void D3Dlight::OnKeyUp(WPARAM keyPressed) {
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
bool D3Dlight::BuildFX()
{
	ifstream fxFile("BasicLight.fxo", ios::binary);
	if (!fxFile)
	{
		MessageBox(NULL, L"fxFile miss!", L"Error", MB_OK);
		return false;
	}
	fxFile.seekg(0, ifstream::end);
	UINT size = static_cast<UINT>(fxFile.tellg());
	fxFile.seekg(0, ifstream::beg);

	vector<char> shader(size);
	fxFile.read(&shader[0], size);
	fxFile.close();
	if (FAILED(D3DX11CreateEffectFromMemory(&shader[0], size, 0, m_d3dDevice, &m_fx)))
	{
		MessageBox(NULL, L"CreateEffect Faild!", L"Error", MB_OK);
		return false;
	}
	m_fxWorldViewProj = m_fx->GetVariableByName("g_worldViewProj")->AsMatrix();
	m_fxWorld = m_fx->GetVariableByName("g_world")->AsMatrix();
	m_fxWorldInvTranspose = m_fx->GetVariableByName("g_worldInvTranspose")->AsMatrix();
	//纹理
	m_fxTexture = m_fx->GetVariableByName("g_texture")->AsShaderResource();
	m_fxTexture->SetResource(m_textureView);

	m_fxPointLight = m_fx->GetVariableByName("g_pointLight");
	m_fxMaterial = m_fx->GetVariableByName("g_material");
	m_fxEyePos = m_fx->GetVariableByName("g_eyePos");

	return true;
}

bool D3Dlight::BuildInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC iDesc[3] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",  0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	ID3DX11EffectTechnique *tech = m_fx->GetTechniqueByName("Light1");
	D3DX11_PASS_DESC pDesc;
	tech->GetPassByIndex(0)->GetDesc(&pDesc);
	if (FAILED(m_d3dDevice->CreateInputLayout(iDesc, 3, pDesc.pIAInputSignature, pDesc.IAInputSignatureSize, &m_inputLayout)))
	{
		MessageBox(NULL, L"CreateInputLayout failed!", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool D3Dlight::BuildBuffers()
{
	GeoGen::CreateGrid(20.f, 20.f, 50, 50, m_grid);
	GeoGen::CreateSphere(2, 40, 30, m_sphere);

	m_gridVStart = 0;    m_gridIStart = 0;
	m_sphereVStart = m_gridVStart + m_grid.vertices.size();
	m_sphereIStart = m_gridIStart + m_grid.indices.size();

	UINT totalVerts = m_sphereVStart + m_sphere.vertices.size();
	UINT totalIndices = m_sphereIStart + m_sphere.indices.size();

	vector<Vertex> vertices(totalVerts);
	vector<UINT> indices(totalIndices);

	for (UINT i = 0; i < m_grid.vertices.size(); i++)
	{
		vertices[m_gridVStart + i].pos = m_grid.vertices[i].pos;
		vertices[m_gridVStart + i].normal = m_grid.vertices[i].normal;
		vertices[m_gridVStart + i].tex = m_grid.vertices[i].tex;
	}
	for (UINT i = 0; i < m_sphere.vertices.size(); i++)
	{
		vertices[m_sphereVStart + i].pos = m_sphere.vertices[i].pos;
		vertices[m_sphereVStart + i].normal = m_sphere.vertices[i].normal;
		vertices[m_sphereVStart + i].tex = m_sphere.vertices[i].tex;
	}
	
	for (UINT i = 0; i < m_grid.indices.size(); i++)
	{
		indices[i + m_gridIStart] = m_grid.indices[i];
	}
	for (UINT i = 0; i < m_sphere.indices.size(); i++)
	{
		indices[i + m_sphereIStart] = m_sphere.indices[i];
	}

	D3D11_BUFFER_DESC vbDesc = { 0 };
	vbDesc.ByteWidth = totalVerts*sizeof(Vertex);
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = &vertices[0];
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	if (FAILED(m_d3dDevice->CreateBuffer(&vbDesc, &vbData, &m_VertexBuffer)))
	{
		MessageBox(NULL, L"CreateVertexBuffer failed!", L"Error", MB_OK);
		return false;
	}
	D3D11_BUFFER_DESC ibDesc = { 0 };
	ibDesc.ByteWidth = sizeof(UINT)*totalIndices;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = &indices[0];
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&ibDesc, &ibData, &m_IndexBuffer)))
	{
		MessageBox(NULL, L"CreateIndexBuffer failed!", L"Error", MB_OK);
		return false;
	}
	return true;
}

