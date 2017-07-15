#pragma once
#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include <Windows.h>
#include <directxmath.h>
using namespace DirectX;
namespace Lights
{
	//��Щ��Դ���Ͷ�����Effect�ж�Ӧ��Դ�Ķ����ϸ��Ӧ

	//ƽ�й�
	struct DirLight
	{
		XMFLOAT4	ambient;	//������
		XMFLOAT4	diffuse;	//�������
		XMFLOAT4	specular;	//�߹�

		XMFLOAT3	dir;		//���շ���
		float		unused;		//������HLSL��"4D����"�������ƥ��
	};

	//���Դ
	struct PointLight
	{
		XMFLOAT4	ambient;	//������
		XMFLOAT4	diffuse;	//�������
		XMFLOAT4	specular;	//�߹�

		XMFLOAT3	pos;		//��Դλ��
		float		range;		//���շ�Χ

		XMFLOAT3	att;		//˥��ϵ��
		float		unused;		//������HLSL��"4D����"�������ƥ��
	};

	//�۹��
	struct SpotLight
	{
		XMFLOAT4	ambient;	//������
		XMFLOAT4	diffuse;	//�������
		XMFLOAT4	specular;	//�߹�

		XMFLOAT3	dir;		//���շ���
		float		range;		//���շ�Χ

		XMFLOAT3	pos;		//��Դλ��
		float		spot;		//�۹�ǿ��ϵ��

		XMFLOAT3	att;		//˥��ϵ��
		float		theta;		//���ɢ�Ƕ�
	};

	//����
	struct Material
	{
		XMFLOAT4	ambient;
		XMFLOAT4	diffuse;
		XMFLOAT4	specular;	//��4��Ԫ��Ϊ���ʵľ��淴��ϵ������������ʱ���Ĺ⻬�̶�
	};
};

#endif