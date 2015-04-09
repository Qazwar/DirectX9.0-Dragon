#include "d3dUtility.h"

IDirect3DDevice9 *Device = 0;
const int Width = 1000;
const int Height = 800;

ID3DXMesh* Objects[4] = {0, 0, 0, 0};
D3DXMATRIX  Worlds[4];
D3DMATERIAL9 Mtrls[4];

bool Setup()
{
	D3DXCreateTeapot(Device, &Objects[0], 0);
	D3DXCreateSphere(Device, 1.0f, 20, 20, &Objects[1], 0);
	D3DXCreateTorus(Device, 0.5f, 1.0f, 20, 20, &Objects[2], 0);
	D3DXCreateCylinder(Device, 0.5f, 0.5f, 2.0f, 20, 20, &Objects[3], 0);

	D3DXMatrixTranslation(&Worlds[0],  0.0f,  2.0f, 0.0f);
	D3DXMatrixTranslation(&Worlds[1],  0.0f, -2.0f, 0.0f);
	D3DXMatrixTranslation(&Worlds[2], -3.0f,  0.0f, 0.0f);
	D3DXMatrixTranslation(&Worlds[3],  3.0f,  0.0f, 0.0f);

	Mtrls[0] = byhj::RED_MTRL;
	Mtrls[1] = byhj::BLUE_MTRL;
	Mtrls[2] = byhj::GREEN_MTRL;
	Mtrls[3] = byhj::YELLOW_MTRL;

	D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
	D3DXCOLOR   c = byhj::WHITE;
	D3DLIGHT9 point = byhj::InitPointLight(&pos, &c);
	Device->SetLight(0, &point);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, false);

	//set the projection matrix
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void CleanUp()
{	for(int i = 0; i < 4; i++)
      byhj::Release<ID3DXMesh*>(Objects[i]);
}

bool render(float timeDelta)
{
	if (Device)
	{
		static float angle  = (3.0f * D3DX_PI) / 2.0f;
		static float height = 5.0f;

		//按键改变view方向，
		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			angle -= 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			angle += 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			height += 5.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			height -= 5.0f * timeDelta;

		D3DXVECTOR3 position( cosf(angle) * 7.0f, height, sinf(angle) * 7.0f );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &view);

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		for(int i = 0; i < 4; i++)
		{
		  Device->SetMaterial(&Mtrls[i]);
		  Device->SetTransform(D3DTS_WORLD, &Worlds[i]);
	      Objects[i]->DrawSubset(0);
		}

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}


//
// WndProc
//
LRESULT CALLBACK byhj::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!byhj::init(hinstance, Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	byhj::EnterMsgLoop(render);

	CleanUp();

	Device->Release();

	return 0;
}
