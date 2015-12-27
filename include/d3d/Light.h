#ifndef D3DLIGHT_H
#define D3DLIGHT_H

#include <d3d11.h>
#include <xnamath.h>

class D3DPointLight
{
public:
	D3DPointLight():m_Color(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), 
		            m_Position(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
	{

	}
	void SetLightColor(XMFLOAT4 color)
	{
		m_Color = color;
	}
	void SetLightPos(XMFLOAT4 pos)
	{
		m_Position = pos;
	}
	XMFLOAT4 GetLightColor()
	{
		return m_Color;
	}
	XMFLOAT4 GetLightPos()
	{
		return m_Position;
	}
private:

	XMFLOAT4 m_Color;
	XMFLOAT4 m_Position;
};
#endif