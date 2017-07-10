#include "Light.fx"

//针对每个物体
cbuffer	PerObject
{
	float4x4	g_worldViewProj;		//世界+视角+投影矩阵
	float4x4	g_world;				//世界矩阵
	float4x4	g_worldInvTranspose;	//世界矩阵的反+转置
	Material	g_material;				//材质
};
//纹理
Texture2D  g_texture;
SamplerState samTex
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = Wrap;
	AddressV = Wrap;
};

//针对每一帧
cbuffer	PerFrame
{
	PointLight  g_pointLight;
	float3		g_eyePos;				//观察点
};

struct VertexIn
{
	float3	pos		: POSITION;		//顶点坐标
	float3	normal	: NORMAL;		//顶点法线
	float2  tex     : TEXCOORD;    //纹理
};
struct VertexOut
{
	float3	posTrans : POSITION;		//世界变换后的坐标
	float4	posH	 : SV_POSITION;		//投影后的坐标
	float3	normal	 : NORMAL;			//世界变换后s的顶点法线
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
	//计算顶点到观察点向量并归一化
	float3 toEye = g_eyePos - pin.posTrans;
	toEye = normalize(toEye);

	//法线归一化
	float3 normal = normalize(pin.normal);

	//最终颜色值：环境光、漫反射光、高光
	//初始清零
	float4 A = float4(1.f,1.f,1.f,1.f);
	float4 D = float4(0.f,0.f,0.f,0.f);
	float4 S = float4(0.f,0.f,0.f,0.f);
	float4 texColor = g_texture.Sample(samTex, pin.tex);
	//逐个光源进行计算
    for (int i = 0; i<numLights; ++i)
	{
		float4 ambient, diff, spec;
		//ComputeDirLight(g_material,g_lights[i],normal,toEye, ambient,diff,spec);
		ComputePointLight(g_material, g_pointLight, normal, pin.posTrans, toEye, ambient, diff, spec);
		A += ambient;
		D += diff;
		S += spec;
	}
	//三元色相加，得到最终颜色值
	float4 litColor = texColor*(A + D) + S;
	//最终颜色透明度使用漫反射光的
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