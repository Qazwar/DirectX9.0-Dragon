#include "d3dUtility.h"

IDirect3DDevice9 *Device = 0;

bool Setup()
{
	return true;
}

void CleanUp()
{

}

bool render(float timeDelta)
{
	if (Device)
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			          0x00000000, 1.0f, 0);
		Device->Present(0, 0, 0, 0); //backbuffer
	}
	return 0;
}

LRESULT CALLBACK byhj::WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		break;
	default:
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE preInstance, PSTR cmdLine, int showCmd)
{
	if (!byhj::init(hinstance, 800, 600, true,  D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "Init3D() FAILED", 0, 0);
		return 0;
	}
	if (!Setup())
	{
		::MessageBox(0, "Setup() --FAILED", 0, 0);
		return 0;
	}
	// render until exit
	byhj::EnterMsgLoop(render);

	CleanUp();
	Device->Release();
	return 0;
}