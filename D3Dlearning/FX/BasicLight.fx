#include "Light.fx"

//���ÿ������
cbuffer	PerObject
{
	float4x4	g_worldViewProj;		//����+�ӽ�+ͶӰ����
	float4x4	g_world;				//�������
	float4x4	g_worldInvTranspose;	//�������ķ�+ת��
	Material	g_material;				//����
};
//����
Texture2D  g_texture;
SamplerState samTex
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = Wrap;
	AddressV = Wrap;
};

//���ÿһ֡
cbuffer	PerFrame
{
	PointLight  g_pointLight;
	float3		g_eyePos;				//�۲��
};

struct VertexIn
{
	float3	pos		: POSITION;		//��������
	float3	normal	: NORMAL;		//���㷨��
	float2  tex     : TEXCOORD;    //����
};
struct VertexOut
{
	float3	posTrans : POSITION;		//����任�������
	float4	posH	 : SV_POSITION;		//ͶӰ�������
	float3	normal	 : NORMAL;			//����任��s�Ķ��㷨��
	float2  tex      : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.posTrans = mul(float4(vin.pos, 1.f), g_world).xyz;
	vout.posH = mul(float4(vin.pos, 1.f), g_worldViewProj);
	vout.normal = mul(vin.normal, (float3x3)g_worldInvTranspose);
	vout.tex = vin.tex;
	return vout;
}

float4 PS(VertexOut pin, uniform int numLights) :SV_TARGET
{
	//���㶥�㵽�۲����������һ��
	float3 toEye = g_eyePos - pin.posTrans;
	toEye = normalize(toEye);

	//���߹�һ��
	float3 normal = normalize(pin.normal);

	//������ɫֵ�������⡢������⡢�߹�
	//��ʼ����
	float4 A = float4(1.f,1.f,1.f,1.f);
	float4 D = float4(0.f,0.f,0.f,0.f);
	float4 S = float4(0.f,0.f,0.f,0.f);
	float4 texColor = g_texture.Sample(samTex, pin.tex);
	//�����Դ���м���
    for (int i = 0; i<numLights; ++i)
	{
		float4 ambient, diff, spec;
		//ComputeDirLight(g_material,g_lights[i],normal,toEye, ambient,diff,spec);
		ComputePointLight(g_material, g_pointLight, normal, pin.posTrans, toEye, ambient, diff, spec);
		A += ambient;
		D += diff;
		S += spec;
	}
	//��Ԫɫ��ӣ��õ�������ɫֵ
	float4 litColor = texColor*(A + D) + S;
	//������ɫ͸����ʹ����������
	litColor.a = texColor.a*g_material.diffuse.a;

	return litColor;
}
technique11 Light0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(0)));
	}
};
technique11 Light1
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(1)));
	}
}

technique11 Light2
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(2)));
	}
}

technique11 Light3
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(3)));
	}
}