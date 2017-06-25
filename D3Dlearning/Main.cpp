#include "D3Dlight.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	D3Dlight saythelight(hInstance);
	saythelight.Init();

	return saythelight.Run();
}

