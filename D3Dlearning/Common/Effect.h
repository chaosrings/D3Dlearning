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
			m_fx->Release();
	}
	bool initEffect(ID3D11Device* pd3d11Device, wstring fxFileName);
	void Effect::setWorldViewProj(CXMMATRIX worldviewproj)
	{
		m_fxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldviewproj));
	}

};
class BasicEffect:public Effect 
{
protected:


	//每个物体
	ID3DX11EffectMatrixVariable		*m_fxWorld;   //世界变换矩阵，在effect中用于求出点到观察点的向量
	ID3DX11EffectMatrixVariable		*m_fxWorldInvTranspose;   //世界变换举证的逆矩阵的转置，effect中用于更新顶点法线
	ID3DX11EffectVariable			*m_fxMaterial;    //材质
	ID3DX11EffectMatrixVariable		*m_fxTexTrans;   //纹理变换矩阵,对应float4x4 g_texTrans
	ID3DX11EffectShaderResourceVariable *m_fxSR;  //纹理，对应effect中的g_tex
	ID3DX11EffectShaderResourceVariable *m_fxCubeSR;   //cube映射
	//针对每一帧
	ID3DX11EffectVariable		    *m_fxPointLight;    //点光源
	ID3DX11EffectVariable			*m_fxEyePos;        //观察点，用于更新观察
	ID3DX11EffectVariable			*m_fxFogColor;
	ID3DX11EffectVariable			*m_fxFogStart;
	ID3DX11EffectVariable			*m_fxFogRange;
	ID3D11InputLayout				*m_inputLayout;
public:

	ID3DX11EffectTechnique          *m_techNoTex;
	ID3DX11EffectTechnique          *m_techNoLight;
	ID3DX11EffectTechnique          *m_techTexLight;
	ID3DX11EffectTechnique			*m_techTexLightFog;
	ID3DX11EffectTechnique			*m_techCubeTexLight;
	BasicEffect() :Effect(),
		m_fxWorld(nullptr), m_fxWorldInvTranspose(nullptr),
		m_fxMaterial(nullptr),m_fxTexTrans(nullptr),

		m_fxPointLight(nullptr),m_fxEyePos(nullptr),
		m_fxSR(nullptr),m_fxCubeSR(nullptr),
		m_techNoTex(nullptr),m_techNoLight(nullptr),m_techTexLight(nullptr),m_techTexLightFog(nullptr),
		m_techCubeTexLight(nullptr)
	{};
	~BasicEffect()
	{}
	bool initBasicEffect(ID3D11Device* pd3d11Device, wstring fxFileName);
	void setWorldMatrix(CXMMATRIX world) { m_fxWorld->SetMatrix(reinterpret_cast<const float*>(&world)); }
	void setWorldInvTranspose(CXMMATRIX worldInvTranspose) { m_fxWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&worldInvTranspose)); }
	void setMaterial(Lights::Material material) { m_fxMaterial->SetRawValue((void*)&material, 0, sizeof(material)); }
	void setTexTrans(CXMMATRIX texTrans) { m_fxTexTrans->SetMatrix(reinterpret_cast<const float*>(&texTrans)); }
	void setPointLight(Lights::PointLight pointlight) 
	{
		m_fxPointLight->SetRawValue((void*)&pointlight, 0, sizeof(pointlight));
	}
	void setEyePos(XMFLOAT3 eyepos) { m_fxEyePos->SetRawValue((void*)&eyepos, 0, sizeof(eyepos)); }
	void setShaderResourceView(ID3D11ShaderResourceView *srv) { m_fxSR->SetResource(srv); }
	void setCubeShaderResourceView(ID3D11ShaderResourceView* cubeSrv) { m_fxCubeSR->SetResource(cubeSrv); }
	void setFogColor(XMFLOAT4 fogColor) { m_fxFogColor->SetRawValue((void*)(&fogColor), 0, sizeof(fogColor)); }
	void setFogStart(float fogStart) { m_fxFogStart->SetRawValue(&fogStart, 0, sizeof(fogStart)); }
	void setFogRange(float fogRange) { m_fxFogRange->SetRawValue(&fogRange, 0, sizeof(fogRange)); }

	ID3DX11Effect* getFX()
	{
		return m_fx;
	}
	ID3D11InputLayout* getInputLayout()
	{
		return m_inputLayout;
	}
};