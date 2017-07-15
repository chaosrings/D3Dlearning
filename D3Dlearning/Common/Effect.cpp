#include "Effect.h"
bool Effect::initEffect(ID3D11Device* pd3d11Device,wstring fxFileName)
{
	ifstream fxFile("basicBlend.fxo", ios::binary);
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
	if (FAILED(D3DX11CreateEffectFromMemory(&shader[0], size, 0, pd3d11Device, &m_fx)))
	{
		MessageBox(NULL, L"CreateEffect Faild!", L"Error", MB_OK);
		return false;
	}
	m_fxWorldViewProj = m_fx->GetVariableByName("g_worldViewProj")->AsMatrix();
	return true;
}



bool BlendEffect::initBlendEffect(ID3D11Device* pd3d11Device, wstring fxFileName)
{
	if (!Effect::initEffect(pd3d11Device, fxFileName))
		return false;
	//物体
	m_fxWorld = m_fx->GetVariableByName("g_world")->AsMatrix();
	m_fxWorldInvTranspose = m_fx->GetVariableByName("g_worldInvTranspose")->AsMatrix();
	m_fxMaterial = m_fx->GetVariableByName("g_material");
	m_fxTexTrans = m_fx->GetVariableByName("g_texTrans")->AsMatrix();
	//纹理
	m_fxSR = m_fx->GetVariableByName("g_tex")->AsShaderResource();

	//帧
	m_fxPointLight = m_fx->GetVariableByName("g_pointLight");
	m_fxEyePos = m_fx->GetVariableByName("g_eyePos");
	m_fxFogColor = m_fx->GetVariableByName("g_fogColor");
	m_fxFogStart = m_fx->GetVariableByName("g_fogStart");
	m_fxFogRange = m_fx->GetVariableByName("g_fogRange");
	//技术
	m_techNoTex = m_fx->GetTechniqueByName("NoTex");
	m_techNoLight = m_fx->GetTechniqueByName("NoLight");
	m_techTexLight = m_fx->GetTechniqueByName("TexLight");
	m_techTexLightFog = m_fx->GetTechniqueByName("TexLightFog");
	
	//
	D3D11_INPUT_ELEMENT_DESC iDesc[3] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",  0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	ID3DX11EffectTechnique *tech = m_fx->GetTechniqueByName("TexLight");
	D3DX11_PASS_DESC pDesc;
	tech->GetPassByIndex(0)->GetDesc(&pDesc);
	if (FAILED(pd3d11Device->CreateInputLayout(iDesc, 3, pDesc.pIAInputSignature, pDesc.IAInputSignatureSize, &m_inputLayout)))
	{
		MessageBox(NULL, L"CreateInputLayout failed!", L"Error", MB_OK);
		return false;
	}
	return true;
}