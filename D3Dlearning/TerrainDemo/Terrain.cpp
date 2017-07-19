#include "Terrain.h"

bool Terrain::InitTerrain(string rawFileName, float maxHeight)
{
	readRawFile(rawFileName, heightMap, nVertsRow * nVertsCol);
	vertices.clear();
	indices.clear();
	//ÿ�ж�������ÿ�ж�����
	//��ʼx��z����
	float oX = -width * 0.5f;
	float oZ = height * 0.5f;
	//ÿһ����������仯
	float dx = width / nCellsRow;
	float dz = height / nCellsCol;

	//������������nVertsRow * nVertsCol
	vertices.resize(nVertsRow * nVertsCol);

	//�����Ӷ���
	for (UINT i = 0; i < nVertsCol; ++i)
	{
		float tmpZ = oZ - dz * i;
		for (UINT j = 0; j < nVertsRow; ++j)
		{
			UINT index = nVertsRow * i + j;
			vertices[index].pos.x = oX + dx * j;
			vertices[index].pos.y = normalizeHeight(getHeightEntry(i, j), maxHeight);
			vertices[index].pos.z = tmpZ;
			vertices[index].normal = XMFLOAT3(0, 1.f, 0);
			vertices[index].tex = XMFLOAT2(dx*i, dx*j);
		}
	}
	//�ܸ�������:m * n
	//�������������: 6 * m * n
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