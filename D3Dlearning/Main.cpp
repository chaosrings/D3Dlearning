//#include "Blending\D3DBlending.h"
//#include "Mirror\MirrorDemo.h"
//#include "CubeMapping\CubeMapping.h"
#include "TerrainDemo\TerrainDemo.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	TerrainDemo db(hInstance);
	if (!db.Init())
		return -1;
	//
	return db.Run();
}

