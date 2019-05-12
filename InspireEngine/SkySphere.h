#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <iostream>
#include <unordered_map>
#include <atlbase.h>

#include "Mesh.h"
#include "Camera.h"
#include "DXShaderManager.h"
#include "SurfaceMaterial.h"
#include "Light.h"

class SkySphere
{
	typedef CComPtr<ID3D11Buffer> ID3D11BufferPtr;

public:
	SkySphere( ID3D11Device &d3d11Device, int LatLines, int LongLines );
	~SkySphere( );
	void Draw(
		ID3D11DeviceContext &d3d11DevCon,
		DXShaderManager &shaderManager,
		Camera &camera );
	void DrawDepth(
		ID3D11DeviceContext &d3d11DevCon,
		DXShaderManager &shaderManager,
		DXShader &shader,
		Camera &camera );

	ID3D11BufferPtr IndexBuffer;
	ID3D11BufferPtr VertexBuffer;
	XMMATRIX World;

private:
	int NumSphereVertices;
	int NumSphereFaces;

	XMMATRIX Rotationx;
	XMMATRIX Rotationz;
	XMMATRIX Rotationy;
};
