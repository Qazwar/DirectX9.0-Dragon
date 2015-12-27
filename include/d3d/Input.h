#ifndef D3DINPUT_H
#define D3DINPUT_H

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include <d3d11.h>
#include <D3DX11.h>
#include <xnamath.h>
#include <dinput.h>
#include <iostream>

namespace byhj
{

namespace d3d
{

const float Pi = 3.1415926535f;

class Camera
{
public:

	Camera():m_Theta(1.5f * Pi), m_Phi(0.25f * Pi), m_Radius(5.0f) 
	{
		m_LastMousePos.x = 0;
		m_LastMousePos.y = 0;

		XMMATRIX I = XMMatrixIdentity();
		XMStoreFloat4x4(&m_World , I);
		XMStoreFloat4x4(&m_View  , I);
		XMStoreFloat4x4(&m_Proj  , I);
	}

	void update();
	void DirectInput(float dt);
	bool InitDirectInput(HWND hWnd,  HINSTANCE hInstance);
    XMFLOAT4X4 GetViewMatrix()
	{
		return m_View;
	}

	void DetectInput(HWND hWnd, double time);

private:

	XMFLOAT4X4 m_World;
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;

	float m_Theta;
	float m_Phi;
	float m_Radius;

	POINT m_LastMousePos;
	float moveLeftRight  ;
	float moveBackForward;

	IDirectInputDevice8* m_pDIKeyboard;
	IDirectInputDevice8* m_pDIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 m_DirectInput;
};

}

}

#endif