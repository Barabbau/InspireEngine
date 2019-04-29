#pragma once

#include <vector>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <atlbase.h>

// Define the SSAO Computation input layout
D3D11_INPUT_ELEMENT_DESC SsaoVertexLayout[ ] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};