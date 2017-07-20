#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <directxmath.h>
#include <windows.h>
#include "AppUtil.h"
using namespace std;
using namespace DirectX;


class Terrain
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	};
		UINT nVertsRow;
		UINT nCellsRow;
		UINT nVertsCol;
		UINT nCellsCol;
		float width;
		float height;
		float maxHeight;
		vector<float> heightMap;
public:
	vector<Vertex> vertices;
	vector<UINT>   indices;
	Terrain(float _width,float _height,int _m,int _n,float _maxHeight){
		width = _width;
		height = _height;
		nCellsRow = _m;
		nCellsCol = _n;
		nVertsRow = nCellsRow + 1;
		nVertsCol = nCellsCol + 1;
		maxHeight = _maxHeight;
	}
	~Terrain() {}
	bool InitTerrain(string rawFileName);
	bool readRawFile(std::string fileName, std::vector<float> & heightMap, int _numVertex)
	{
		std::vector<char> in(_numVertex);
		heightMap.resize(_numVertex);
		std::ifstream inFile(fileName.c_str(), std::ios_base::binary);
		if (!inFile)
			return false;
		inFile.read(&in[0], _numVertex);
		inFile.close();
		for (unsigned int i = 0; i < in.size(); ++i)
			heightMap[i] = normalizeHeight(in[i]);
		return true;
	}
	float getHeightByPosition(float x, float z);  // 根据 x,z的坐标获得高度信息
	float  getHeightEntry(int row, int col)   //根据row col获得高度信息
	{
		return heightMap[row*nVertsRow + col];
	}
	float normalizeHeight(int grayPicHeight)
	{
		return (1.f*grayPicHeight/256)* maxHeight;
	}
	void computeNormal(Vertex& v1, Vertex &v2, Vertex &v3)
	{
		XMFLOAT3 f1(v2.pos.x - v1.pos.x, v2.pos.y - v1.pos.y, v2.pos.z - v1.pos.z);
		XMFLOAT3 f2(v3.pos.x - v1.pos.x, v3.pos.y - v1.pos.y, v3.pos.z - v1.pos.z);
		XMVECTOR vec1 = XMLoadFloat3(&f1);
		XMVECTOR vec2 = XMLoadFloat3(&f2);
		XMVECTOR temp = XMVector3Normalize(XMVector3Cross(vec1, vec2));
		XMFLOAT3 tempNormal;
		XMStoreFloat3(&tempNormal, temp);
		v1.normal = tempNormal;
		v2.normal = tempNormal;
		v3.normal = tempNormal;
	}
	
};
