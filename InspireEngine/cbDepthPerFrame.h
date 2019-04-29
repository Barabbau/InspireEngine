#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

/// Updating Parameter for Effect_CreateDepthMap.fx
struct cbDepthPerFrame
{
	XMMATRIX		WorldViewProjection;
};