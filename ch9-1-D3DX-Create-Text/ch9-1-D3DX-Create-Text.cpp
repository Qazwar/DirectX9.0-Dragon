#include "d3dUtility.h"

IDirect3DDevice9 *Device = 0;
const int Width = 1000;
const int Height = 800;

ID3DXMesh *Text = 0;

bool Setup()
{
	//get a handle  to a device
	HDC hdc = CreateCompatibleDC(0);
	HFONT hFont;
	HFONT hFontOld;

	// Describe the font we want.
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfHeight         = 25;    // in logical units
	lf.lfWidth          = 12;    // in logical units
	lf.lfEscapement     = 0;        
	lf.lfOrientation    = 0;     
	lf.lfWeight         = 500;   // boldness, range 0(light) - 1000(bold)
	lf.lfItalic         = false;   
	lf.lfUnderline      = false;    
	lf.lfStrikeOut      = false;    
	lf.lfCharSet        = DEFAULT_CHARSET;
	lf.lfOutPrecision   = 0;              
	lf.lfClipPrecision  = 0;          
	lf.lfQuality        = 0;           
	lf.lfPitchAndFamily = 0;    
	strcpy(lf.lfFaceName, "Times New Roman"); // font style


	// Create the font and select it with the device context.
	hFont = CreateFontIndirect(&lf);
	hFontOld = (HFONT)SelectObject(hdc, hFont); 

	// Create the text mesh based on the selected font in the HDC.
	D3DXCreateText(Device, hdc, "Direct3D", 0.001f, 0.4f, &Text, 0, 0);

	// Restore the old font and free the acquired HDC.
	SelectObject(hdc, hFontOld);
	DeleteObject( hFont );
	DeleteDC( hdc );

	// Lights.
	//

	D3DXVECTOR3 dir(0.0f, -0.5f, 1.0f);
	D3DXCOLOR col = byhj::WHITE;
	D3DLIGHT9 light = byhj::InitDirectionalLight(&dir, &col);
	Device->SetLight(0, &light);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//set camera
	D3DXVECTOR3 pos(0.0f, 1.5f, -3.3f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(
		&V,
		&pos,
		&target,
		&up);

	Device->SetTransform(D3DTS_VIEW, &V);

	//set project
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f, // 45 - degree
		(float)Width / (float)Height,
		0.01f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

bool render(float timeDelta)
{
	if (Device)
	{
		D3DXMATRIX yRot, T;
		static float y = 0.0f;
		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;

		if( y >= 6.28f )
			y = 0.0f;
		D3DXMatrixTranslation(&T, -1.6f, 0.0f, 0.0f);
		T = T * yRot;
		Device->SetTransform(D3DTS_WORLD, &T);

		// Render
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		Device->SetMaterial(&byhj::WHITE_MTRL);
		Text->DrawSubset(0);

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


// WinMain
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
	Device->Release();

	return 0;
}

