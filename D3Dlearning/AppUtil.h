#pragma once
#ifndef _APP_UTIL_H_
#define _APP_UTIL_H_

#include <Windows.h>
#include <directxmath.h>
using namespace DirectX;
template<typename T>
void SafeRelease(T t)
{
	if (t)
	{
		t->Release();
		t = NULL;
	}
}

template<typename T>
T Clamp(T vMin, T vMax, T value)
{
	value = max(vMin, value);
	value = min(vMax, value);

	return value;
}

inline int KeyDown(int vKey)
{
	return GetAsyncKeyState(vKey) & 0x8000;
}

namespace Colors
{
	//���弸����������ɫֵ�������ڳ�����ʹ��
	const XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	const XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	const XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	const XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	const XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	const XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	const XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	const XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
	const XMVECTORF32 Silver = { 0.75f,0.75f,0.75f,1.0f };
};
#define takeoffSpeed 14.f

#endif