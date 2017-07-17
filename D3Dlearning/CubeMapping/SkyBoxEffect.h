#pragma once
#include "Effect.h"
class SkyBoxEffect :public Effect
{
protected:
	
	ID3DX11EffectShaderResourceVariable *m_fxSRVCubeMap;
	ID3D11InputLayout                   *m_inputLayout;
public:
	ID3DX11EffectTechnique *m_skyBoxTech;
	SkyBoxEffect():Effect(){ m_skyBoxTech = nullptr, m_fxSRVCubeMap = nullptr, m_inputLayout = nullptr; }
	~SkyBoxEffect() {
		SafeRelease(m_skyBoxTech);
		SafeRelease(m_fxSRVCubeMap);
		SafeRelease(m_inputLayout);
	};
	bool initSkyBoxEffect(ID3D11Device* device, wstring fxFileName)
	{
		if (!Effect::initEffect(device, fxFileName))
			return false;
		m_fxSRVCubeMap = m_fx->GetVariableByName("g_cubeMap")->AsShaderResource();
		m_skyBoxTech = m_fx->GetTechniqueByName("SkyBoxTech");
		const D3D11_INPUT_ELEMENT_DESC iDesc[1] =
		{
			{ "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		ID3DX11EffectTechnique *tech =m_skyBoxTech;
		D3DX11_PASS_DESC pDesc;
		tech->GetPassByIndex(0)->GetDesc(&pDesc);
		if (FAILED(device->CreateInputLayout(iDesc, 1, pDesc.pIAInputSignature, pDesc.IAInputSignatureSize, &m_inputLayout)))
		{
			MessageBox(NULL, L"CreateInputLayout failed!", L"Error", MB_OK);
			return false;
		}
		return true;
	}
	void setShaderResourceView(ID3D11ShaderResourceView *srv) { m_fxSRVCubeMap->SetResource(srv); }
	ID3D11InputLayout*  getInputLayout() const { return m_inputLayout; }
	ID3DX11Effect*  getFX() const { return m_fx; }
};