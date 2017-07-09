#pragma once
#include <d3dx11effect.h>
#include <string>
#include <vector>
#include <fstream>
#include <d3d11_1.h>
#include "Lights.h"
#include "AppUtil.h"
#include "DDSTextureLoader.h"
using namespace std;
class Effect
{
protected:
	ID3DX11Effect						*m_fx;
	ID3DX11EffectMatrixVariable			*m_fxWorldViewProj;
public:
	Effect() :m_fx(nullptr), m_fxWorldViewProj(nullptr) {};
	virtual ~Effect()
	{
		if (m_fx != nullptr)
			m_fx->Release;
		if (m_fxWorldViewProj != nullptr)
			m_fxWorldViewProj->Release;
	}
	bool initEffect(ID3D11Device* pd3d11Device, wstring fxFileName);
	void setWorldViewProj(XMMATRIX worldviewproj);
};
class BlendEffect:public Effect 
{
protected:
	//每个物体
	ID3DX11EffectMatrixVariable		*m_fxWorld;   //世界变换矩阵，在effect中用于求出点到观察点的向量
	ID3DX11EffectMatrixVariable		*m_fxWorldInvTranspose;   //世界变换举证的逆矩阵的转置，effect中用于更新顶点法线
	ID3DX11EffectVariable			*m_fxMaterial;    //材质
	ID3DX11EffectMatrixVariable		*m_fxTexTrans;   //纹理变换矩阵,对应float4x4 g_texTrans
	ID3DX11EffectShaderResourceVariable *m_fxSR;  //纹理，对应effect中的g_tex

	//针对每一帧
	ID3DX11EffectVariable		    *m_fxPointLight;    //点光源
	ID3DX11EffectVariable			*m_fxEyePos;        //观察点，用于更新观察点

	ID3DX11EffectTechnique          *m_techNoTex;
	ID3DX11EffectTechnique          *m_techNoLight;
	ID3DX11EffectTechnique          *m_techTexLight;
	 


	ID3D11InputLayout				*m_inputLayout;
private:
	bool initInputLayout(ID3D11Device* pd3d11Device);
	

public:
	BlendEffect() :Effect(),
		m_fxWorld(nullptr), m_fxWorldInvTranspose(nullptr),
		m_fxMaterial(nullptr),m_fxTexTrans(nullptr),

		m_fxPointLight(nullptr),m_fxEyePos(nullptr),
		m_fxSR(nullptr),
		m_techNoTex(nullptr),m_techNoLight(nullptr),m_techTexLight(nullptr)
	{};
	~BlendEffect()
	{
		SafeRelease(m_fxWorld);
		SafeRelease(m_fxWorldInvTranspose);
		SafeRelease(m_fxMaterial);
		SafeRelease(m_fxTexTrans);
		SafeRelease(m_fxPointLight);
		SafeRelease(m_fxEyePos);
		SafeRelease(m_fxSR);
		SafeRelease(m_techNoTex);
		SafeRelease(m_techNoLight);
		SafeRelease(m_techTexLight);

		SafeRelease(m_inputLayout);
	}

	bool initBlendEffect(ID3D11Device* pd3d11Device, wstring fxFileName);
	void setWorldMatrix(XMMATRIX world) { m_fxWorld->SetMatrix(reinterpret_cast<float*>(&world)); }
	void setWorldInvTranspose(XMMATRIX worldInvTranspose) { m_fxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose)); }
	void setMaterial(Lights::Material material) { m_fxMaterial->SetRawValue(&material, 0, sizeof(Lights::Material)); }
	void setTexTrans(XMMATRIX texTrans) { m_fxTexTrans->SetMatrix(reinterpret_cast<float*>(&texTrans)); }
	void setPointLight(Lights::PointLight pointlight) 
	{
		m_fxPointLight->SetRawValue(&pointlight, 0, sizeof(Lights::PointLight));
	}
	void setEyePos(XMFLOAT3 eyepos) { m_fxEyePos->SetRawValue(&eyepos, 0, sizeof(eyepos)); }
	void setShaderResourceView(ID3D11ShaderResourceView *srv) { m_fxSR->SetResource(srv); }
};