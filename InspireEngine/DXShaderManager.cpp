#include "DXShaderManager.h"

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
	ZeroMemory( &cbbd, sizeof( D3D11_BUFFER_DESC ) );

	cbbdInstanced.Usage = D3D11_USAGE_DEFAULT;
	cbbdInstanced.ByteWidth = sizeof( cbPerObject ) * 512;
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

	hr = d3d11Device.CreateBuffer( &cbbd, NULL, &cbPerFrameBuffer );





	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof( sampDesc ) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = d3d11Device.CreateSamplerState( &sampDesc, &TexSamplerState );



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

	d3d11Device.CreateBlendState( &blendDesc, &d2dTransparency );

	///////////////**************new**************////////////////////
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

	d3d11Device.CreateBlendState( &blendDesc, &Transparency );
}

DXShaderManager::~DXShaderManager( )
{
	cbPerFrameBuffer.Release( );
	cbPerObjectBuffer.Release( );
	cbPerObjectBufferInstanced.Release( );

	Transparency.Release( );
}