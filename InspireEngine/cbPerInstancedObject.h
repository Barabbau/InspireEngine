#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

struct cbPerInstancedObject
{
	public:
	XMMATRIX  WVP;
	XMMATRIX World[ 512 ];

	//These will be used for the pixel shader
	XMFLOAT4 difColor;
	bool hasTexture;
};