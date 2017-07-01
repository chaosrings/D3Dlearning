#pragma once
#pragma once
#ifndef _WIN_APP_H_
#define _WIN_APP_H_

#include <Windows.h>
#include <string>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include "Timer.h"
#include "DDSTextureLoader.h"
class WinApp
{
public:
	WinApp(HINSTANCE hInst, std::wstring title = L"D3D11ѧϰ������", int width = 1024, int height = 720);
	virtual ~WinApp();

	//����������Ա����
	HINSTANCE	AppInstance()	const { return m_hInstance; }
	HWND		Window()		const { return m_hWnd; }
	int			Width()			const { return m_clientWidth; }
	int			Height()		const { return m_clientHeight; }
	void		SetWindowTitle(std::wstring title) { SetWindowText(m_hWnd, title.c_str()); }

	/*
	����������д��Щ������ʵ���Զ���Ĺ���
	���ڸ�����������дʱ��Ҫ�ȵ��ø���ĺ�����������Զ���Ĺ��ܣ�
	���磺Init(),������Init()�У���Ҫ�ȵ���WinApp::Init()��
	ͬ��Ҳ�ʺ���OnResize()��
	*/
	virtual bool	Init();							//�����ʼ��
	virtual bool	OnResize();						//�����ڴ�С�ı�ʱ����
	virtual bool	Update(float timeDelt) = 0;		//ÿ֡����
	virtual bool	Render() = 0;					//��Ⱦ

	virtual LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

	int		Run();		//��ѭ��

						//�������������������¡��ͷš��ƶ�
						//Ĭ��ʲôҲ������������д��ʵ���ض�����
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }
	virtual void OnKeyDown(WPARAM keyPressed) {}
	virtual void OnKeyUp(WPARAM keyPressed) {}
protected:
	bool	InitWindow();		//��ʼ��Win32����
	bool	InitD3D();			//��ʼ��D3D11

	void	CalculateFPS();		//����֡��
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
protected:
	HINSTANCE	m_hInstance;		//Ӧ�ó���ʵ�����
	HWND		m_hWnd;				//���ھ��

	int			m_clientWidth;		//���ڴ�С
	int			m_clientHeight;


	bool		m_isMinimized;		//�Ƿ���С��
	bool		m_isMaximized;		//�Ƿ����
	bool		m_isPaused;			//�Ƿ���ͣ����
	bool		m_isResizing;		//��������ڸı䴰�ڳߴ�ʱ

	ID3D11Device			*m_d3dDevice;				//D3D11�豸
	ID3D11DeviceContext		*m_deviceContext;			//�豸������
	IDXGISwapChain			*m_swapChain;				//������
	ID3D11Texture2D			*m_depthStencilBuffer;		//���/ģ�建����
	ID3D11RenderTargetView	*m_renderTargetView;		//��Ⱦ������ͼ
	ID3D11DepthStencilView	*m_depthStencilView;		//���/ģ����ͼ

	std::wstring	m_winTitle;			//��������
	Timer			m_timer;			//Ӧ�ó���ʱ��

private:
	//���⸴��
	WinApp(const WinApp&);
	WinApp& operator = (const WinApp&);
};

#endif	//_WIN_APP_H_