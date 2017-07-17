#include "Light.fx"
cbuffer PerObject
{
	float4x4 g_worldViewProj;
	float4x4 g_world;
	float4x4 g_worldInvTranspose;
	Material g_material;
	float4x4 g_texTrans;
};
Texture2D g_tex;
SamplerState samplerTex
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = Wrap;
	AddressV = Wrap;
};
TextureCube g_cubeTex;
cbuffer PerFrame
{
	PointLight g_pointLight;
	float3     g_eyePos;
	float4     g_fogColor;
	float      g_fogStart;
	float      g_fogRange;
};


//变换前的坐标，法线，纹理坐标
struct VertexIn
{
	float3	pos		: POSITION;		//顶点坐标
	float3	normal	: NORMAL;		//顶点法线
	float2  tex     : TEXCOORD;    //纹理
};
//经过世界视角透视变换后的位置，法线，纹理坐标
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
	vout.tex = mul(float4(vin.tex, 0.f, 1.f), g_texTrans).xy;
	
	return vout;
}

float4 PS(VertexOut pin,uniform int useLight,uniform bool useTexture,uniform bool useFog,uniform bool useReflect):SV_TARGET
{
	float4 texColor = float4(1.f,1.f,1.f,1.f);
	float dist = 0.f;
	if (useTexture)
	{	
		texColor = g_tex.Sample(samplerTex, pin.tex);
	}
	float4 litColor = texColor;
	float3 toEye = g_eyePos - pin.posTrans;
	dist = length(toEye);
	if (useFog)
	{
		clip(g_fogStart + g_fogRange - dist);
	}
	toEye = normalize(toEye);
	float3 normal = normalize(pin.normal);
	float4 A = float4(0.f, 0.f, 0.f, 0.f);
	float4 D = float4(0.f, 0.f, 0.f, 0.f);
	float4 S = float4(0.f, 0.f, 0.f, 0.f);
	if (useLight)
	{
		float4 ambient, diff, spec;
		ComputePointLight(g_material, g_pointLight, normal, pin.posTrans, toEye, ambient, diff, spec);
		A += ambient;
		D += diff;
		S += spec;
	}
	else
	{
		A = g_material.ambient;
		D = g_material.diffuse;
		S = g_material.specular;
	}
	litColor = texColor * (A + D) + S;
	litColor.a = texColor.a*g_material.diffuse.a;
	if (useFog)
	{
		float fogFactor = saturate((dist - g_fogStart) / g_fogRange);
		litColor = lerp(litColor, g_fogColor, fogFactor);
	}
	if (useReflect)
	{
		float3 ref = reflect(-toEye, normal);
		float4 refColor = g_cubeTex.Sample(samplerTex, ref);
		litColor = lerp(litColor, refColor, 0.82f);
	}
	return litColor;
}

technique11 NoTex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true, false,false,false)));
	}
}
technique11 NoLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(false, true,false,false)));
	}
}

technique11 TexLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true, true,false,false)));
	}
}
technique11 TexLightFog
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true, true, true,false)));
	}
}
technique11 CubeTexLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true, true, false, true)));
	}
};