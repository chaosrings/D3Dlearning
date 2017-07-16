#include "Blending\D3DBlending.h"
//#include "Mirror\MirrorDemo.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{

	D3DBlending db(hInstance);
	if (!db.Init())
		return -1;
	//
	return db.Run();
}

