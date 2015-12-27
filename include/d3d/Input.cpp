#include "Input.h"

#define SHADER_DEBUG

namespace byhj
{
namespace d3d
{

float Clamp(const float& x, const float& low, const float& high)
{
	return x < low ? low : (x > high ? high : x); 
}

void Camera::update()
{
	// Convert Spherical to Cartesian coordinates.
	float x = m_Radius * sinf(m_Phi) * cosf(m_Theta);
	float z = m_Radius * sinf(m_Phi) * sinf(m_Theta);
	float y = m_Radius * cosf(m_Phi);

	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 0.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX View = XMMatrixLookAtLH(pos, target, up);

	XMStoreFloat4x4(&m_View, XMMatrixTranspose(View) );
}


bool Camera::InitDirectInput(HWND hWnd, HINSTANCE hInstance)
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_DirectInput,
		NULL); 

	hr = m_DirectInput->CreateDevice(GUID_SysKeyboard, &m_pDIKeyboard, NULL);
	hr = m_DirectInput->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);

	hr = m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pDIMouse->SetDataFormat(&c_dfDIMouse);

#ifdef SHADER_DEBUG
	hr = m_pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND);
	hr = m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND);
#else
	hr = m_pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	hr = m_pDIMouse->SetCooperativeLevel(hWnd,  DISCL_NOWINKEY | DISCL_FOREGROUND);
#endif

	return true;
}

void Camera::DetectInput(HWND hWnd, double time)
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	//Acquire current mouse and key state
	m_pDIKeyboard->Acquire();
	m_pDIMouse->Acquire();
	m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	m_pDIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

	if(keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hWnd, WM_DESTROY, 0, 0);

	float speed = 15.0f * time;
	float x = 0.0f, y = 0.0f;
	//Enter ADSW to move camera left right back forword

	if(keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if(keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}
	if(keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += speed;
	}
	if(keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= speed;
	}
	//Use mouse to change the rotation matrix
	if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		x -= mouseCurrState.lX;
		y -= mouseCurrState.lY;
		if( (mouseCurrState.rgbButtons[0] & 0x80) )
		{
			// Make each pixel correspond to a quarter of a degree.
			float dx = XMConvertToRadians(x);
			float dy = XMConvertToRadians(y);
			// Update angles based on input to orbit camera around box.
			m_Theta += dx;
			m_Phi   += dy;
			// Restrict the angle mPhi.
			m_Phi = Clamp(m_Phi, 0.1f, Pi - 0.1f);
		}

		m_LastMousePos.x = x;
		m_LastMousePos.y = y;

		mouseLastState = mouseCurrState;
	}
	update();

	return;
}

}


}