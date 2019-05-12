#pragma once

#include <vector>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <atlbase.h>

#include "DXShader.h"
#include "cbPerObject.h"
#include "cbPerObjectInstanced.h"
#include "cbPerFrame.h"
#include "cbDepthPerFrame.h"

class DXShaderManager
{
	typedef CComPtr<ID3D11Buffer> ID3D11BufferPtr;
	typedef CComPtr<ID3D11BlendState> ID3D11BlendStatePtr;
	typedef CComPtr<ID3D11SamplerState> ID3D11SamplerStatePtr;

	public:
	DXShaderManager( );
	DXShaderManager( ID3D11Device &d3d11Device );
	~DXShaderManager( );

	ID3D11SamplerStatePtr TexSamplerState;
	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	// Constant Buffer Buffers
	ID3D11BufferPtr cbPerFrameBuffer;
	ID3D11BufferPtr cbDepthPerFrameBuffer;
	ID3D11BufferPtr cbPerObjectBuffer;
	ID3D11BufferPtr cbPerObjectBufferInstanced;

	// Constant Buffer structures
	cbPerFrame cbPerFrame;
	cbDepthPerFrame cbDepthPerFrame;
	cbPerObject cbPerObj;
	cbPerObjectInstanced cbPerObjInstanced;
	

	ID3D11BlendStatePtr Transparency;
	ID3D11BlendStatePtr D2dTransparency;
	ID3D11BlendStatePtr	D3dAlphaBlendState;
	ID3D11BlendStatePtr D3dDepthTestsBlendState;
	ID3D11BlendStatePtr InspireDefault;

	// Skybox texture
	ID3D11ShaderResourceView* skyboxTexture;

	DXShader* _stdShader;
	DXShader* _stdShaderInstanced;
	DXShader* _flatColorShader;

	private:

};