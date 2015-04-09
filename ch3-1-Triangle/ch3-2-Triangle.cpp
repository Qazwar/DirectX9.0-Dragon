#include "d3dUtility.h"

IDirect3DDevice9 *Device = 0;
const int Width = 1000;
const int Height = 800;

IDirect3DVertexBuffer9 *Triangle = 0;

struct Vertex
{
	Vertex(){}
	Vertex( float x, float y, float z)
	{
		_x = x; _y = y; _z = z;  
	}
	float _x, _y, _z;
	static const DWORD FVF; //静态成员在外面定义
};

const DWORD Vertex::FVF = D3DFVF_XYZ;


bool Setup()
{
	//Create the Vertex Buffer
	Device->CreateVertexBuffer(
		3 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY, // usage
		Vertex::FVF,        // Vertex format
		D3DPOOL_MANAGED,    // managed memory pool
		&Triangle,          // return create vertex buffer
		0);

	//fill the vertex buffer
	Vertex *VertexData;
	Triangle->Lock(0, 0, (void**)&VertexData, 0);

	VertexData[0] = Vertex(-1.0f, 0.0f, 2.0f);
	VertexData[1] = Vertex( 0.0f, 1.0f, 2.0f);
	VertexData[2] = Vertex( 1.0f, 0.0f, 2.0f);

	Triangle->Unlock();

	//set the projection matrix
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	//set wireframe mode render status
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	
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

		Device->SetStreamSource(0, Triangle, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
        //draw the triangle
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
