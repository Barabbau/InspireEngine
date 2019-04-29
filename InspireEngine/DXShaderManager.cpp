#include "DXShaderManager.h"
#include "InspireUtils.h"

DXShaderManager::DXShaderManager( )
{
	// ctor
}

DXShaderManager::DXShaderManager( ID3D11Device &d3d11Device )
{
	HRESULT hr = 0;

	// Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory( &cbbd, sizeof( D3D11_BUFFER_DESC ) );

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof( cbPerObject );
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device.CreateBuffer( &cbbd, NULL, &cbPerObjectBuffer );


	// Create the buffer to send to the cbuffer in effect file
	// Instanced version
	D3D11_BUFFER_DESC cbbdInstanced;
	ZeroMemory( &cbbdInstanced, sizeof( D3D11_BUFFER_DESC ) );

	cbbdInstanced.Usage = D3D11_USAGE_DEFAULT;
	cbbdInstanced.ByteWidth = sizeof( cbPerObjectInstanced );
	cbbdInstanced.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbdInstanced.CPUAccessFlags = 0;
	cbbdInstanced.MiscFlags = 0;

	hr = d3d11Device.CreateBuffer( &cbbdInstanced, NULL, &cbPerObjectBufferInstanced );


	// Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory( &cbbd, sizeof( D3D11_BUFFER_DESC ) );

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof( cbPerFrame );
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device.CreateBuffer( &cbbd, NULL, &this->cbPerFrameBuffer );


	// Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory( &cbbd, sizeof( D3D11_BUFFER_DESC ) );

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof( cbDepthPerFrame );
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device.CreateBuffer( &cbbd, NULL, &this->cbDepthPerFrameBuffer );

	

	/*
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mTexture));
	*/

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof( sampDesc ) );
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;// D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 2;// D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = d3d11Device.CreateSamplerState( &sampDesc, &TexSamplerState );


	ID3D11BlendState *d2dBlendState;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof( blendDesc ) );

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof( rtbd ) );

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[ 0 ] = rtbd;

	d3d11Device.CreateBlendState( &blendDesc, &d2dBlendState );

	ID3D11BlendStatePtr D2dTransparencyTemp( d2dBlendState );
	this->D2dTransparency = D2dTransparencyTemp;


	///////////////**************new**************////////////////////
	ID3D11BlendState *transparency;

	ZeroMemory( &rtbd, sizeof( rtbd ) );

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[ 0 ] = rtbd;

	d3d11Device.CreateBlendState( &blendDesc, &transparency );

	ID3D11BlendStatePtr TransparencyTemp( transparency );
	this->Transparency = TransparencyTemp;



	// Setup Depth Shader Blend
// create blend state
	ID3D11BlendState *d3dDepthTestsBlendState;
	D3D11_BLEND_DESC stDesc;

	ZeroMemory( &stDesc, sizeof( D3D11_BLEND_DESC ) );

	stDesc.RenderTarget[ 0 ].BlendEnable = true;
	stDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
	stDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ZERO;
	stDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_MIN;
	stDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	stDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
	stDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	stDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;

	d3d11Device.CreateBlendState( &stDesc, &d3dDepthTestsBlendState );
	ID3D11BlendStatePtr d3dDepthTestsBlendStatePtr( d3dDepthTestsBlendState );
	this->D3dDepthTestsBlendState = d3dDepthTestsBlendStatePtr;

	/*
	// Setup Default Blend
	D3D11_BLEND_DESC ltDesc;

	ZeroMemory( &ltDesc, sizeof( D3D11_BLEND_DESC ) );

	ltDesc.RenderTarget[ 0 ].BlendEnable = true;
	ltDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
	ltDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ZERO;
	ltDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	ltDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	ltDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
	ltDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	ltDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pd3dDevice->CreateBlendState( &ltDesc, &D3DDefaultBlendState );
	*/
	// Setup FullScreen Alpha Blend
	ID3D11BlendState *d3dAlphadBlendState;

	D3D11_BLEND_DESC omDesc;
	ZeroMemory( &omDesc, sizeof( D3D11_BLEND_DESC ) );

	D3D11_RENDER_TARGET_BLEND_DESC rtbdAlpha;
	ZeroMemory( &rtbdAlpha, sizeof( rtbdAlpha ) );

	rtbdAlpha.BlendEnable = true;
	rtbdAlpha.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbdAlpha.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbdAlpha.BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	rtbdAlpha.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbdAlpha.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbdAlpha.BlendOpAlpha = D3D11_BLEND_OP_SUBTRACT;
	rtbdAlpha.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

//	omDesc.AlphaToCoverageEnable = false;
	omDesc.RenderTarget[ 0 ] = rtbdAlpha;
	ID3D11BlendStatePtr d3dAlphadBlendStateTemp( d3dAlphadBlendState );
	this->D3dAlphaBlendState = d3dAlphadBlendStateTemp;




}

DXShaderManager::~DXShaderManager( )
{
	cbPerFrameBuffer.Release( );
	cbPerObjectBuffer.Release( );
	cbPerObjectBufferInstanced.Release( );

	Transparency.Release( );
}