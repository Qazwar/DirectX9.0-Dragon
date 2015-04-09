#include "d3dUtility.h"

IDirect3DDevice9 *Device = 0;
const int Width = 1000;
const int Height = 800;

IDirect3DVertexBuffer9 *Triangle = 0;
D3DXMATRIX WorldMatrix;

struct ColorVertex
{
	ColorVertex(){}
	ColorVertex( float x, float y, float z, D3DCOLOR c)
	{
		_x = x; _y = y; _z = z; 
		_color = c;
	}
	float _x, _y, _z;
	D3DCOLOR _color;
	static const DWORD FVF; //静态成员在外面定义
};

const DWORD ColorVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

bool Setup()
{
	//Create the Vertex Buffer
	Device->CreateVertexBuffer(
		3 * sizeof(ColorVertex),
		D3DUSAGE_WRITEONLY, // usage
		ColorVertex::FVF,        // Vertex format
		D3DPOOL_MANAGED,    // managed memory pool
		&Triangle,          // return create vertex buffer
		0);

	//fill the vertex buffer
	ColorVertex* v;
	Triangle->Lock(0, 0, (void**)&v, 0);

	v[0] = ColorVertex(-1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(255,   0,   0));
	v[1] = ColorVertex( 0.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(  0, 255,   0));
	v[2] = ColorVertex( 1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(  0,   0, 255));

	Triangle->Unlock();

	//投影矩阵
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	Device->SetRenderState(D3DRS_LIGHTING, false); //普通的填充模式
	
	return true;
}

void CleanUp()
{
	byhj::Release<IDirect3DVertexBuffer9*>(Triangle);
}

bool render(float timeDelta)
{
	if (Device)
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		
		Device->BeginScene();

		Device->SetFVF(ColorVertex::FVF);
		Device->SetStreamSource(0, Triangle, 0, sizeof(ColorVertex)); //数据流

		// draw the triangle to the left with flat shading
		D3DXMatrixTranslation(&WorldMatrix, -1.25f, 0.0f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &WorldMatrix);
		Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		// draw the triangle to the right with gouraud shading
		D3DXMatrixTranslation(&WorldMatrix, 1.25f, 0.0f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &WorldMatrix);
		Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

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
