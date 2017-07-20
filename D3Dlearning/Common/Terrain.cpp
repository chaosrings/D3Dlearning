#include "Terrain.h"

float Terrain::getHeightByPosition(float x, float z)
{
	float cellSpaceX = width / nCellsRow;
	float cellSpaceZ = height / nCellsCol;
	
	float distanceX = x + 0.5f*width;
	float distanceZ = 0.5f*height-z;
	
	if (distanceX > width||distanceZ>height)
		return 0;
	int row = floorf(distanceZ / cellSpaceZ);
	int col = floorf(distanceX / cellSpaceX);
	/*float x0 = 1.f*col*cellSpaceX - 0.5f*width;
	float z0 = 1.f*(row + 1)*cellSpaceZ - 0.5f*height;
	float k = cellSpaceZ / cellSpaceX;
	float judge = k*x - z + z0 - k*x0;*/
	float A = getHeightEntry(row, col);
	float D = getHeightEntry(row + 1, col + 1);

	float ux = x - (1.f*col*cellSpaceX - 0.5f*width);
	float uz = z - (0.5f*height-1.f*row*cellSpaceZ);
	float distanceLerp = sqrtf(ux*ux + uz*uz);
	float distanceCross = sqrtf(cellSpaceX*cellSpaceX + cellSpaceZ*cellSpaceZ);
	float pos = lerpf(A, D, (distanceLerp / distanceCross));
	
	return pos;

}
bool Terrain::InitTerrain(string rawFileName)
{
	readRawFile(rawFileName, heightMap, nVertsRow * nVertsCol);
	vertices.clear();
	indices.clear();
	//每行顶点数、每列顶点数
	//起始x、z坐标
	float oX = -width * 0.5f;
	float oZ = height * 0.5f;
	//每一格纹理坐标变化
	float dx = width / nCellsRow;
	float dz = height / nCellsCol;

	//顶点总数量：nVertsRow * nVertsCol
	vertices.resize(nVertsRow * nVertsCol);

	//逐个添加顶点
	for (UINT i = 0; i < nVertsCol; ++i)
	{
		float tmpZ = oZ - dz * i;
		for (UINT j = 0; j < nVertsRow; ++j)
		{
			UINT index = nVertsRow * i + j;
			vertices[index].pos.x = oX + dx * j;
			vertices[index].pos.y = getHeightEntry(i, j);
			vertices[index].pos.z = tmpZ;
			vertices[index].normal = XMFLOAT3(0, 1.f, 0);
			vertices[index].tex = XMFLOAT2(dx*i, dx*j);
		}
	}
	//总格子数量:m * n
	//因此总索引数量: 6 * m * n
	UINT nIndices = nCellsRow * nCellsCol * 6;
	indices.resize(nIndices);
	UINT tmp = 0;
	for (UINT i = 0; i < nCellsCol; ++i)
	{
		for (UINT j = 0; j < nCellsRow; ++j)
		{
			indices[tmp] = i * nVertsRow + j;
			indices[tmp + 1] = i * nVertsRow + j + 1;
			indices[tmp + 2] = (i + 1) * nVertsRow + j;
			computeNormal(vertices[indices[tmp]], vertices[indices[tmp + 1]], vertices[indices[tmp + 2]]);
			indices[tmp + 3] = i * nVertsRow + j + 1;
			indices[tmp + 4] = (i + 1) * nVertsRow + j + 1;
			indices[tmp + 5] = (i + 1) * nVertsRow + j;
			computeNormal(vertices[indices[tmp + 3]], vertices[indices[tmp + 4]], vertices[indices[tmp + 5]]);
			tmp += 6;
		}
	}
	return true;
}