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

class DXShaderManager
{
	typedef CComPtr<ID3D11Buffer> ID3D11BufferPtr;
	typedef CComPtr<ID3D11BlendState> ID3D11BlendStatePtr;

	public:
	DXShaderManager( );
	DXShaderManager( ID3D11Device &d3d11Device );
	~DXShaderManager( );

	ID3D11SamplerState* TexSamplerState;


	ID3D11BufferPtr cbPerFrameBuffer;
	ID3D11BufferPtr cbPerObjectBuffer;
	ID3D11BufferPtr cbPerObjectBufferInstanced;

	
	cbPerObject cbPerObj;
	cbPerObjectInstanced cbPerObjInstanced;
	cbPerFrame constbuffPerFrame;

	ID3D11BlendStatePtr Transparency;
	ID3D11BlendStatePtr d2dTransparency;

	// Skybox texture
	ID3D11ShaderResourceView* skyboxTexture;

	DXShader* _stdShader;
	DXShader* _stdShaderInstanced;

	private:

};