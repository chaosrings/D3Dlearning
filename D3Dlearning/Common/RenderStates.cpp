#include "AppUtil.h"
#include "RenderStates.h"
ID3D11RasterizerState* RenderStates::WireFrameRS(NULL);
ID3D11RasterizerState* RenderStates::NoCullRS(NULL);
ID3D11RasterizerState* RenderStates::CounterClockFrontRS(NULL);

ID3D11BlendState* RenderStates::TransparentBS(NULL);
ID3D11BlendState* RenderStates::NoColorWriteBS(NULL);

ID3D11DepthStencilState* RenderStates::MarkMirrorDSS(NULL);
ID3D11DepthStencilState* RenderStates::DrawReflectionDSS(NULL);
ID3D11DepthStencilState* RenderStates::NoDoubleBlendDDS(NULL);
bool RenderStates::InitRenderStates(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC wfDesc;
	ZeroMemory(&wfDesc, sizeof(wfDesc));
	wfDesc.FillMode = D3D11_FILL_WIREFRAME;
	wfDesc.CullMode = D3D11_CULL_BACK;
	wfDesc.FrontCounterClockwise = false;
	wfDesc.DepthClipEnable = true;
	if (FAILED(device->CreateRasterizerState(&wfDesc, &WireFrameRS)))
	{
		MessageBox(NULL, L"Create 'WireFrame' rasterizer state failed!", L"Error", MB_OK);
		return false;
	}

	D3D11_BLEND_DESC transDesc;
	transDesc.AlphaToCoverageEnable = false;        //关闭AlphaToCoverage  
	transDesc.IndependentBlendEnable = false;       //不针对多个RenderTarget使用不同的混合状态  
													//因此只设置第一个数组元素即可  
	transDesc.RenderTarget[0].BlendEnable = true;
	transDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	if (FAILED(device->CreateBlendState(&transDesc, &TransparentBS)))
	{
		MessageBox(NULL, L"Create 'transparentBS state failed!'", L"Error", MB_OK);
		return false;
	}
	D3D11_RASTERIZER_DESC  ncDesc;
	ZeroMemory(&ncDesc, sizeof(ncDesc));
	ncDesc.CullMode = D3D11_CULL_NONE;  //取消消除背面
	ncDesc.FillMode = D3D11_FILL_SOLID;
	ncDesc.FrontCounterClockwise = false;
	ncDesc.DepthClipEnable = true;
	if (FAILED(device->CreateRasterizerState(&ncDesc, &NoCullRS)))
	{
		MessageBox(NULL, L"Create 'NoCull' rasterizer state failed!", L"Error", MB_OK);
		return false;
	}
	
	D3D11_RASTERIZER_DESC ccfDesc;
	ZeroMemory(&ccfDesc, sizeof(ccfDesc));
	ccfDesc.CullMode = D3D11_CULL_BACK;
	ccfDesc.FillMode = D3D11_FILL_SOLID;
	ccfDesc.FrontCounterClockwise = true;     //设置逆时针为正面
	ccfDesc.DepthClipEnable = true;
	if (FAILED(device->CreateRasterizerState(&ccfDesc, &CounterClockFrontRS)))
	{
		MessageBox(NULL, L"Create 'NoCull' rasterizer state failed!", L"Error", MB_OK);
		return false;
	}

	D3D11_BLEND_DESC noColorWriteBlendDesc;
	noColorWriteBlendDesc.AlphaToCoverageEnable = false;
	noColorWriteBlendDesc.IndependentBlendEnable = false;
	noColorWriteBlendDesc.RenderTarget[0].BlendEnable = false;
	noColorWriteBlendDesc.RenderTarget[0].RenderTargetWriteMask = 0;  //不允许颜色写入
	if (FAILED(device->CreateBlendState(&noColorWriteBlendDesc, &NoColorWriteBS)))
	{
		MessageBox(NULL, L"Create 'No Color Write' blend state failed!", L"Error", MB_OK);
		return false;
	}


	//填充镜子区域
	D3D11_DEPTH_STENCIL_DESC markMirrorDSSDesc;
	//深度测试
	markMirrorDSSDesc.DepthEnable = true;
	markMirrorDSSDesc.DepthFunc = D3D11_COMPARISON_LESS;
	markMirrorDSSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	//模版测试
	markMirrorDSSDesc.StencilEnable = true;
	markMirrorDSSDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	markMirrorDSSDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	//关键
	markMirrorDSSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;  //总是能通过测试
	markMirrorDSSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; //通过模版测试后将模版缓存的值设为ref
	markMirrorDSSDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  //
	markMirrorDSSDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//因为背面消除所以backFace没什么用
	markMirrorDSSDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	markMirrorDSSDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	markMirrorDSSDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	markMirrorDSSDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	if (FAILED(device->CreateDepthStencilState(&markMirrorDSSDesc, &MarkMirrorDSS)))
	{
		MessageBox(NULL, L"Create 'MarkMirror' depth stencil state failed!", L"Error", MB_OK);
		return false;
	}

	//绘制镜子中物体
	D3D11_DEPTH_STENCIL_DESC drawRefDesc;
	drawRefDesc.DepthEnable = true;
	drawRefDesc.DepthFunc = D3D11_COMPARISON_LESS;
	drawRefDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	drawRefDesc.StencilEnable = true;
	drawRefDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	drawRefDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	//正面设置是关键
	drawRefDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;    //只有在相等的时候才通过测试
	drawRefDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;    //通过测试后不改变模版缓存的值
	drawRefDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;    //测试失败。保持不变
	drawRefDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	drawRefDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	drawRefDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	drawRefDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawRefDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	if (FAILED(device->CreateDepthStencilState(&drawRefDesc, &DrawReflectionDSS)))
	{
		MessageBox(NULL, L"Create 'DrawReflection' depth stencil state failed!", L"Error", MB_OK);
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC nodoubleblendDesc;
	nodoubleblendDesc.DepthEnable = true;
	nodoubleblendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	nodoubleblendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	nodoubleblendDesc.StencilEnable = true;
	nodoubleblendDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	nodoubleblendDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	nodoubleblendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER;
	nodoubleblendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	nodoubleblendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	nodoubleblendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	nodoubleblendDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	nodoubleblendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	nodoubleblendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	nodoubleblendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	if (FAILED(device->CreateDepthStencilState(&nodoubleblendDesc, &NoDoubleBlendDDS)))
	{
		MessageBox(NULL, L"Create 'NoDoubleBlendDDS' failed!", L"Error", MB_OK);
		return false;
	}
	return true;
}
void RenderStates::Release()
{
	SafeRelease(NoCullRS);
	SafeRelease(CounterClockFrontRS);
	SafeRelease(TransparentBS);
	SafeRelease(NoColorWriteBS);
	SafeRelease(MarkMirrorDSS);
	SafeRelease(DrawReflectionDSS);
}