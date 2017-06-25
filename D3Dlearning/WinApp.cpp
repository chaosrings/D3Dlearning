#include "WinApp.h"
#include "AppUtil.h"
#include <iostream>
#include <sstream>
#include <WindowsX.h>
#include <d3d11_1.h>
namespace {
	WinApp  *g_winApp(NULL);
	UINT    g_x4MsaaQuality;
}
LRESULT CALLBACK WinAppProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_winApp->WinProc(hwnd, uMsg, wParam, lParam);
}
WinApp::WinApp(HINSTANCE hInst, std::wstring title, int width, int height) :m_hInstance(hInst),
m_hWnd(NULL),
m_winTitle(title),
m_clientWidth(width),
m_clientHeight(height),
m_isMinimized(false),
m_isMaximized(false),
m_isPaused(false),
m_isResizing(false),
m_d3dDevice(NULL),
m_deviceContext(NULL),
m_swapChain(NULL),
m_renderTargetView(NULL),
m_depthStencilBuffer(NULL),
m_depthStencilView(NULL)
{
	//��ʼ��ȫ�ֶ���ָ�룩
	g_winApp = this;

	//	//��debugģʽ�£����Ǵ򿪿���̨����ʾһЩ���õ���Ϣ
	//#if defined(debug) || defined(_debug)
	//	file *f(null);
	//	if (allocconsole())
	//	{
	//		freopen_s(&f, "conout$", "w", stdout);
	//	}
	//#endif
}
WinApp::~WinApp()
{
	SafeRelease(m_depthStencilView);
	SafeRelease(m_renderTargetView);
	SafeRelease(m_swapChain);
	SafeRelease(m_depthStencilBuffer);

	if (m_deviceContext)
		m_deviceContext->ClearState();
	SafeRelease(m_deviceContext);
	SafeRelease(m_d3dDevice);
}
bool WinApp::Init()
{
	if (!InitWindow())
		return false;
	if (!InitD3D())
		return false;

	return true;
}

bool WinApp::InitWindow()
{
	WNDCLASS wndcls;
	wndcls.cbClsExtra = 0;
	wndcls.cbWndExtra = 0;
	wndcls.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndcls.hInstance = m_hInstance;
	wndcls.lpfnWndProc = WinAppProc;
	wndcls.lpszClassName = m_winTitle.c_str();
	wndcls.lpszMenuName = NULL;
	wndcls.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wndcls))
	{
		MessageBox(NULL, L"Register class failed!", L"ERROR", MB_OK);
		return false;
	}

	RECT winRect = { 0,0,m_clientWidth,m_clientHeight };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, false);
	int width = winRect.right - winRect.left;
	int height = winRect.bottom - winRect.top;
	m_hWnd = CreateWindow(m_winTitle.c_str(),
		m_winTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		m_hInstance,
		NULL);
	if (!m_hWnd)
	{
		MessageBox(NULL, L"Create Window failed!", L"ERROR", MB_OK);
		return false;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return true;
}
bool WinApp::InitD3D()
{
	HRESULT	hr;

	D3D_FEATURE_LEVEL featureLevels[6] = { D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL	curLevel;
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, featureLevels, 6, D3D11_SDK_VERSION, &m_d3dDevice, &curLevel, &m_deviceContext);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Craete device failed!", L"ERROR", MB_OK);
		return false;
	}

	if (curLevel != D3D_FEATURE_LEVEL_11_0)
	{
		if (IDNO == MessageBox(NULL, L"���Ļ�����֧��D3D11���ԣ���������޷���ȷ��ʾЧ����Ҫ������", L"����", MB_YESNO))
		{
			return false;
		}
	}

	m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &g_x4MsaaQuality);

#if defined(DEBUG) || defined(_DEBUG)
	std::string level("Unkown");
	switch (curLevel)
	{
	case D3D_FEATURE_LEVEL_11_0:
		level = "11_0";
		break;
	case D3D_FEATURE_LEVEL_10_1:
		level = "10_1";
		break;
	case D3D_FEATURE_LEVEL_10_0:
		level = "10_0";
		break;
	case D3D_FEATURE_LEVEL_9_3:
		level = "9_3";
		break;
	case D3D_FEATURE_LEVEL_9_2:
		level = "9_2";
		break;
	case D3D_FEATURE_LEVEL_9_1:
		level = "9_1";
		break;
	}
#endif

	DXGI_SWAP_CHAIN_DESC scDesc = { 0 };
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferDesc.Width = m_clientWidth;
	scDesc.BufferDesc.Height = m_clientHeight;
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDesc.BufferCount = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.Flags = 0;
	scDesc.OutputWindow = m_hWnd;
	scDesc.SampleDesc.Count = g_x4MsaaQuality<1 ? 1 : 4;
	scDesc.SampleDesc.Quality = g_x4MsaaQuality<1 ? 0 : g_x4MsaaQuality - 1;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Windowed = true;

	IDXGIDevice *pDxgiDevice(NULL);
	hr = m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Get DXGIDevice failed!", L"ERROR", MB_OK);
		return false;
	}
	IDXGIAdapter *pDxgiAdapter(NULL);
	hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Get DXGIAdapter failed!", L"ERROR", MB_OK);
		return false;
	}
	IDXGIFactory *pDxgiFactory(NULL);
	hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pDxgiFactory));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Get DXGIFactory failed!", L"ERROR", MB_OK);
		return false;
	}
	hr = pDxgiFactory->CreateSwapChain(m_d3dDevice, &scDesc, &m_swapChain);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create swap chain failed!", L"ERROR", MB_OK);
		return false;
	}
	SafeRelease(pDxgiFactory);
	SafeRelease(pDxgiAdapter);
	SafeRelease(pDxgiDevice);

	m_swapChain->ResizeBuffers(1, m_clientWidth, m_clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	ID3D11Texture2D *backBuffer(NULL);
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	hr = m_d3dDevice->CreateRenderTargetView(backBuffer, 0, &m_renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create render target view failed!", L"ERROR", MB_OK);
		return false;
	}
	backBuffer->Release();

	D3D11_TEXTURE2D_DESC dsDesc;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.Width = m_clientWidth;
	dsDesc.Height = m_clientHeight;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.SampleDesc.Count = g_x4MsaaQuality < 1 ? 1 : 4;
	dsDesc.SampleDesc.Quality = g_x4MsaaQuality < 1 ? 0 : g_x4MsaaQuality - 1;
	dsDesc.MiscFlags = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = m_d3dDevice->CreateTexture2D(&dsDesc, 0, &m_depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create depth stencil buffer failed!", L"ERROR", MB_OK);
		return false;
	}
	hr = m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create depth stencil view failed!", L"ERROR", MB_OK);
		return false;
	}

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	D3D11_VIEWPORT viewPort;
	viewPort.Width = static_cast<FLOAT>(m_clientWidth);
	viewPort.Height = static_cast<FLOAT>(m_clientHeight);
	viewPort.MaxDepth = 1.f;
	viewPort.MinDepth = 0.f;
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	m_deviceContext->RSSetViewports(1, &viewPort);


	return true;
}
//���ڳߴ�ı�ʱ�����´���RenderTarget��DepthStencilTarget�������ӿڱ任
bool WinApp::OnResize()
{
	if (m_swapChain == NULL)
		return false;
	//HRESULT hr;

	//SafeRelease(m_depthStencilView);
	//SafeRelease(m_renderTargetView);
	//SafeRelease(m_depthStencilBuffer);



	return true;
}
LRESULT CALLBACK WinApp::WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		//LOWORD(wParam): WA_ACTIVE, WA_CLICKACTIVE, WA_INACTIVE
		//HIWORD(wParam): isMinimezed(bool)
		//lParam: HWND, window being activated
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_isPaused = true;
			m_timer.Stop();
		}
		else
		{
			m_isPaused = false;
			m_timer.Start();
		}
		return 0;

	case WM_SIZE:
		//���ڳߴ�ı�
		//������Ӧ����
		m_clientWidth = LOWORD(lParam);
		m_clientHeight = HIWORD(lParam);
		//������ڱ���С��
		if (wParam == SIZE_MINIMIZED)
		{
			m_isMaximized = false;
			m_isMinimized = true;
			m_isPaused = true;
		}
		//������ڱ����
		else if (wParam == SIZE_MAXIMIZED)
		{
			m_isMaximized = true;
			m_isMinimized = false;
			OnResize();
		}
		//������ڸ�ԭ
		else if (wParam == SIZE_RESTORED)
		{
			//���->��ԭ
			if (m_isMaximized)
			{
				m_isMaximized = false;
				m_isPaused = false;
				OnResize();
			}
			//��С��->��ԭ
			else if (m_isMinimized)
			{
				m_isMinimized = false;
				m_isPaused = false;
				OnResize();
			}
			else
			{
				if (!m_isResizing)
				{
					OnResize();
				}
			}
		}
		return 0;

		//��꿪ʼ�϶����ڱ�Ե���ı��С
	case WM_ENTERSIZEMOVE:
		m_isPaused = true;
		m_isResizing = true;
		m_timer.Stop();
		return 0;

		//����϶����ڴ�С����
	case WM_EXITSIZEMOVE:
		m_isPaused = false;
		m_isResizing = false;
		m_timer.Start();
		OnResize();
		return 0;

		//��걻����ʱ
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		this->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;


		//����ͷ�ʱ
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

		//����ƶ�ʱ
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

		//���ƴ��ڣ�����̫С
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 150;
		return 0;

		//��alt+enterʱ�����ᷢ������
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_DESTROY:
		//Exit
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		OnKeyDown(wParam);
		return 0;
	case WM_KEYUP:
		OnKeyUp(wParam);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
void WinApp::CalculateFPS()
{
	static float begin = m_timer.TotalTime();
	static int frameCounter = 0;
	if (m_timer.TotalTime() - begin >= 1.f)
	{
		std::wostringstream text;
		text << L"      FPS: " << frameCounter << L"    FrameTime: " << 1000.f / frameCounter << L"ms";
		SetWindowTitle(m_winTitle + text.str());
		begin = m_timer.TotalTime();
		frameCounter = 0;
	}
	++frameCounter;
}
int  WinApp::Run()
{
	MSG msg = { 0 };

	m_timer.Reset();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (1 == 1)
			{
				//��ʱ������
				m_timer.Tick();
				//����֡��
				CalculateFPS();
				//����
				Update(m_timer.DeltaTime());
				//��Ⱦ
				Render();
			}
			else
			{
				//������ͣʱ�����䡰˯�ߡ����Խ�ʡCPU
				Sleep(200);
			}
		}
	}

	//�����˳�
	return msg.wParam;
}
HRESULT WinApp::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}