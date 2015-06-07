#include "d3dUtility.h"
#include <stdio.h>
#include <vector>

IDirect3DDevice9 *Device = 0;
const int Width = 1000;
const int Height = 800;

ID3DXMesh*                      SourceMesh = 0;
ID3DXPMesh*                     PMesh      = 0; // progressive mesh
std::vector<D3DMATERIAL9>       Mtrls(0);
std::vector<IDirect3DTexture9*> Textures(0);


bool Setup()
{
	HRESULT hr = 0;
	ID3DXBuffer* adjBuffer  = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls   = 0;

	hr = D3DXLoadMeshFromX(  
		"../media/object/bigship1.x",
		D3DXMESH_MANAGED,
		Device,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&SourceMesh);


	if( mtrlBuffer != 0 && numMtrls != 0 )
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(int i = 0; i < numMtrls; i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// save the ith material
			Mtrls.push_back( mtrls[i].MatD3D );

			// check if the ith material has an associative texture
			if( mtrls[i].pTextureFilename != 0 )
			{
				// yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					Device,
					mtrls[i].pTextureFilename,
					&tex);

				// save the loaded texture
				Textures.push_back( tex );
			}
			else
			{
				// no texture for the ith subset
				Textures.push_back( 0 );
			}
		}
	}

	hr = SourceMesh->OptimizeInplace(		
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT  |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		(DWORD*)adjBuffer->GetBufferPointer(), // new adjacency info
		0, 0);

	if(FAILED(hr))
	{
		::MessageBox(0, "OptimizeInplace() - FAILED", 0, 0);
		byhj::Release<ID3DXBuffer*>(adjBuffer); // free
		return false;
	}


	// Generate the progressive mesh. 
	hr = D3DXGeneratePMesh(
		SourceMesh,
		(DWORD*)adjBuffer->GetBufferPointer(), // adjacency
		0,                  // default vertex attribute weights
		0,                  // default vertex weights
		1,                  // simplify as low as possible
		D3DXMESHSIMP_FACE,  // simplify by face count
		&PMesh);

	byhj::Release<ID3DXMesh*>(SourceMesh);  // done w/ source mesh
	byhj::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXGeneratePMesh() - FAILED", 0, 0);
		return false;
	}

	// set to original detail
	DWORD maxFaces = PMesh->GetMaxFaces();
	PMesh->SetNumFaces(maxFaces);

	//
	// Set texture filters.
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 
	// Set Lights.
	//

	D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
	D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = byhj::InitDirectionalLight(&dir, &col);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	// Set camera.
	D3DXVECTOR3 pos(-8.0f, 4.0f, -12.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(
		&V,
		&pos,
		&target,
		&up);

	Device->SetTransform(D3DTS_VIEW, &V);

	// Set projection matrix.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

bool render(float timeDelta)
{
	if (Device)
	{
		// Get the current number of faces the pmesh has.
		int numFaces = PMesh->GetNumFaces();

		// Add a face, note the SetNumFaces() will  automatically
		// clamp the specified value if it goes out of bounds.
		if( ::GetAsyncKeyState('A') & 0x8000f )
		{
			// Sometimes we must add more than one face to invert
			// an edge collapse transformation
			PMesh->SetNumFaces( numFaces + 1 );
			if( PMesh->GetNumFaces() == numFaces )
				PMesh->SetNumFaces( numFaces + 2 );
		}

		// Remove a face, note the SetNumFaces() will  automatically
		// clamp the specified value if it goes out of bounds.
		if( ::GetAsyncKeyState('S') & 0x8000f )
			PMesh->SetNumFaces( numFaces - 1 );

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		for(int i = 0; i < Mtrls.size(); i++)
		{
			// draw pmesh
			Device->SetMaterial( &Mtrls[i] );
			Device->SetTexture(0, Textures[i]);
			PMesh->DrawSubset(i);

			// draw wireframe outline
			Device->SetMaterial(&byhj::YELLOW_MTRL);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			PMesh->DrawSubset(i);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
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

