#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

struct cbPerObject
{
	public:
	XMMATRIX  WVP;
	XMMATRIX World;

	//These will be used for the pixel shader
	XMFLOAT4 difColor;
	bool hasTexture;
};
