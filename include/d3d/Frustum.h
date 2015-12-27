#ifndef Frustum_H
#define Frustum_H

#include <D3DX11.h>
#include <d3d11.h>
#include <D3DX10math.h>
#include <xnamath.h>

namespace byhj
{

namespace d3d
{


class Frustum
{
public:
	Frustum() {}
	Frustum(const Frustum &) {}
	~Frustum() {}

	void Construct(float screenDepth, D3DXMATRIX proj , D3DXMATRIX view);
	
	bool CheckPoint(float x, float y, float z);
	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);

private:
	D3DXPLANE m_Planes[6];
};

}

}

#endif