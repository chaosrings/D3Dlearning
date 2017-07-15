#pragma once
#include <d3d11_1.h>
class RenderStates
{
public:
	static bool InitRenderStates(ID3D11Device* device);
	static void Release();
	static ID3D11RasterizerState  *NoCullRS;
	static ID3D11RasterizerState  *CounterClockFrontRS;

	static ID3D11BlendState		  *TransparentBS;       //透明效果
	static ID3D11BlendState		  *NoColorWriteBS;     //不写入颜色
	
	static ID3D11DepthStencilState* MarkMirrorDSS;
	static ID3D11DepthStencilState* DrawReflectionDSS;
	static ID3D11DepthStencilState* NoDoubleBlendDDS;
};