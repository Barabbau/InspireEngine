#include "SsaoManager.h"
#include "SsaoVertexLayout.h"

SsaoManager::SsaoManager( 
	ID3D11Device  &d3d11Device,
	ID3D11DeviceContext &d3d11DevCon,
	LPCWSTR shaderFileName,
	DXShaderManager &shaderManager, 
	float width,
	float height )
	: ShaderManager( shaderManager )
{
	this->Intialize( d3d11Device, d3d11DevCon, 1, shaderFileName, width, height );
}

SsaoManager::~SsaoManager( )
{
}

///<summary>
///Compile Vertex and Pixel Shaders
///<para>Assign the Input Layout ( POSITION, TEXCOORD, ETC.. )</para>
///<para>The function assumes the usage of Shader Model 4</para>
///<para>The function assumes that the Vertex Shader function name is VS</para>
///<para>The function assumes that the Pixel Shader function name is PS</para>
///</summary>
///<param name='d3d11DevCon'>the Context</param>
///<param name='d3d11Device'>A pointer to the actual Hardware device</param>
///<param name='numElements'>the numer of Shader Layout inputs</param>
///<param name='shaderFileName'>the Shader file name</param>
///<param name='shaderLayout'>the Shader Layout</param>
///<returns>Returns OK or the failure value for Debug</returns>
HRESULT  SsaoManager::Intialize( 
	ID3D11Device  &d3d11Device,
	ID3D11DeviceContext &d3d11DevCon,
	int numElements, 
	LPCWSTR shaderFileName,
	float width,
	float height )
{
	HRESULT hr = S_OK;

	this->SsaoDisplayShader = new DXShader( "VS", "PS", *SsaoVertexLayout, numElements, d3d11Device, L"Effect_ComputeSSAOMap.fx" );
	this->SsaoCreateShader = new DXShader( "VS", "PS", *SsaoVertexLayout, numElements, d3d11Device, L"Effect_CreateDepthMap.fx" );

	hr = D3DX11CreateShaderResourceViewFromFile(
		&d3d11Device,
		L"Resources\\maps\\SSAONormals.dds",
		NULL,
		NULL,
		&this->TexRandomNormal,
		NULL );
	
	this->UpdateResources( d3d11Device, width, height );

	//
	// Init Vertex Buffer
	//
	//	1__3
	//	|\ |
	//	| \|
	//	0--2
	SsaoVertex QuadVertices[ ] =
	{
		{ XMFLOAT3( 0.0f,  1.0f, 0.0f ) },
		{ XMFLOAT3( 0.0f,  0.0f, 0.0f ) },
		{ XMFLOAT3( 1.0f,  1.0f, 0.0f ) },
		{ XMFLOAT3( 1.0f,  0.0f, 0.0f ) },

	};

	// Create the vertex buffer
	D3D11_BUFFER_DESC vertexBufferDescription;
	ZeroMemory( &vertexBufferDescription, sizeof( vertexBufferDescription ) );

	// write access access by CPU and GPU
	vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	// size is the VERTEX struct * 4
	vertexBufferDescription.ByteWidth = sizeof( SsaoVertex ) * 4;
	// use as a vertex buffer
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;    
	// allow CPU to write in buffer
	vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	ID3D11Buffer* screenVertexBuffer;

	d3d11Device.CreateBuffer( &vertexBufferDescription, 0, &screenVertexBuffer );
	ID3D11BufferPtr screenSpriteCostantBufferPtr( screenVertexBuffer );
	this->ScreenVertexBuffer = screenVertexBuffer;

	// Copy Vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE		ms;

	// map the buffer
	d3d11DevCon.Map( this->ScreenVertexBuffer.p, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms );
	// copy the data
	memcpy( ms.pData, QuadVertices, sizeof( QuadVertices ) );
	// unmap the buffer
	d3d11DevCon.Unmap( this->ScreenVertexBuffer.p, NULL );


	//
	// Full Screen Quad Sprite Indexes
	//
	WORD indices[ ] =
	{
		0,1,2,
		2,1,3,
	};

	// Index buffer description
	D3D11_BUFFER_DESC				indexDesc;

	ZeroMemory( &indexDesc, sizeof( indexDesc ) );

	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.ByteWidth = sizeof( WORD ) * 6;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0;

	// Index Data
	D3D11_SUBRESOURCE_DATA			indexData;

	ZeroMemory( &indexData, sizeof( indexData ) );
	indexData.pSysMem = indices;

	ID3D11Buffer* screenIndexBuffer;

	d3d11Device.CreateBuffer( &indexDesc, &indexData, &screenIndexBuffer );
	ID3D11BufferPtr screenIndexBufferPtr( screenIndexBuffer );
	this->ScreenIndexBuffer = screenIndexBufferPtr;

	//
	// Create the constant buffer for SSAO Computation
	//
	D3D11_BUFFER_DESC ssaoDesc;

	ZeroMemory( &ssaoDesc, sizeof( ssaoDesc ) );

	ssaoDesc.Usage = D3D11_USAGE_DEFAULT;
	ssaoDesc.ByteWidth = sizeof( ScreenSSAOComputeCBChangesEveryFrame );
	ssaoDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;


	ID3D11Buffer* ssaoConstantBuffer;

	d3d11Device.CreateBuffer( &ssaoDesc, 0, &ssaoConstantBuffer );
	ID3D11BufferPtr ssaoCostantBufferPtr( ssaoConstantBuffer );
	this->SsaoCBChangesEveryFrame = ssaoCostantBufferPtr;

	return hr;
}

///<summary>
///Compute and Blend the SSAO
///</summary>
///<param name='d3d11Device'>A pointer to the actual Hardware device</param>
///<param name='d3d11DevCon'>A pointer to the Device Context</param>
void SsaoManager::Draw( ID3D11Device  &d3d11Device,
						ID3D11DeviceContext &d3d11DevCon,
						float nearClip,
						float farClip,
						float width,
						float height )
{
	/*
	ID3D11RenderTargetView *target;
	ID3D11DepthStencilView *targetDepth;

	d3d11DevCon.OMGetRenderTargets( 1, &target, &targetDepth );

	CD3D11_RENDER_TARGET_VIEW_DESC targetDesc;
	target->GetDesc( &targetDesc );
	*/

	// Vertex buffer
	d3d11DevCon.VSSetShader( this->SsaoDisplayShader->VS, nullptr, 0 );
	d3d11DevCon.PSSetShader( this->SsaoDisplayShader->PS, nullptr, 0 );


	UINT stride = sizeof( SsaoVertex );
	UINT offset = 0;
	
	d3d11DevCon.IASetVertexBuffers( 0, 1, &this->ScreenVertexBuffer.p, &stride, &offset );


	// Primtive type
	d3d11DevCon.IASetIndexBuffer( this->ScreenIndexBuffer.p, DXGI_FORMAT_R16_UINT, 0 );
	//d3d11DevCon.IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	XMMATRIX view = XMMatrixIdentity( );
	XMMATRIX projection = XMMatrixOrthographicOffCenterLH( 0.0f, width, height, 0.0f, nearClip, farClip ); //XMMatrixOrthographicOffCenterLH( 0.0f, width, height, 0.0f, 0.0f, 100.0f );
	XMMATRIX worldMatrix = XMMatrixMultiply( view, projection );


	ScreenSSAOComputeCBChangesEveryFrame shaderParameters;

	// Prepare World Coordinates to send to the shader
	XMMATRIX scale = XMMatrixScaling( width, height, 1.0f );
	XMMATRIX WVP = scale;
	XMMATRIX projectionWorldMatrix = XMMatrixMultiply( WVP, worldMatrix );

	XMStoreFloat4x4( &shaderParameters.worldMatrix, XMMatrixTranspose( projectionWorldMatrix ) );

	XMVECTOR testDistScale = XMVectorSet( 320.0f, nearClip, farClip, 100.0f ); //320.0f, 100.0f, 100.0f, 100.0f );
	XMStoreFloat4( &shaderParameters.distanceScale, testDistScale );

	// defines the max UV shift for the SSAO iteration samples
	XMVECTOR testRadius = XMVectorSet( 0.0055f, 0.35f, 0.35f, 0.35f );
	XMStoreFloat4( &shaderParameters.sampleRadius, testRadius );


	// Set Alpha Blending Mode for Transparency
	d3d11DevCon.OMSetBlendState(this->ShaderManager.D3dAlphaBlendState, 0, 0xffffffff );
	
	//
	// Set the Vertex Layout
	//
	d3d11DevCon.IASetInputLayout( this->SsaoDisplayShader->VertLayout );
	
	
	// The Layer To Show
	d3d11DevCon.PSSetShaderResources( 0, 1, &this->DepthShaderResource );
	d3d11DevCon.PSSetShaderResources( 1, 1, &this->TexRandomNormal );
	d3d11DevCon.PSSetSamplers( 0, 1, &this->ShaderManager.TexSamplerState.p );

	d3d11DevCon.UpdateSubresource( this->SsaoCBChangesEveryFrame.p, 0, NULL, &shaderParameters, 0, 0 );

	d3d11DevCon.VSSetConstantBuffers( 0, 1, &this->SsaoCBChangesEveryFrame.p );

	d3d11DevCon.PSSetConstantBuffers( 0, 1, &this->SsaoCBChangesEveryFrame.p );

	d3d11DevCon.DrawIndexed( 6, 0, 0 );
	
	// Set back the Default Blend State
	d3d11DevCon.OMSetBlendState( 0, 0, 0xffffffff );//D3DDefaultBlendState
}

///<summary>
///Create the resources for the personalized depth test
///<para>The depth test is used for the SSAO computation</para>
///</summary>
///<param name='d3d11Device'>A pointer to the actual Hardware device</param>
///<returns>Returns OK or the failure value for Debug</returns>
HRESULT SsaoManager::UpdateResources( 
	ID3D11Device &d3d11Device,
	float width,
	float height )
{
	HRESULT hr = S_OK;
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = nullptr; } }
#endif
	SAFE_RELEASE( this->DepthTexture );
	SAFE_RELEASE( this->DepthRenderTarget );
	SAFE_RELEASE( this->DepthShaderResource );

	//
	// Create the Depth Texture
	//
	D3D11_TEXTURE2D_DESC	textureDesc;
	ZeroMemory( &textureDesc, sizeof( textureDesc ) );
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;//D3D11_BIND_DEPTH_STENCIL | 
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;


	hr = d3d11Device.CreateTexture2D( &textureDesc, NULL, &this->DepthTexture );
	if ( FAILED( hr ) )
		return hr;


	//
	// Create the Depth Render Target
	//
	D3D11_RENDER_TARGET_VIEW_DESC	renderTargetViewDesc;

	renderTargetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	hr = d3d11Device.CreateRenderTargetView( this->DepthTexture, &renderTargetViewDesc, &this->DepthRenderTarget );
	if ( FAILED( hr ) )
		return hr;


	//
	// Create the Depth Shader Resource
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC						shaderResourceViewDesc;

	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = d3d11Device.CreateShaderResourceView( this->DepthTexture, &shaderResourceViewDesc, &this->DepthShaderResource );
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

///<summary>
/// Set Depoth target
///</summary>
///<param name='d3d11Device'>A pointer to the actual Hardware device</param>
///<param name='d3d11DevCon'>A pointer to the Device Context</param>
void SsaoManager::SetupDepthRenderTarget( 
	ID3D11Device  &d3d11Device,
	ID3D11DeviceContext &d3d11DevCon,
	float farClip )
{
	d3d11DevCon.OMSetRenderTargets( 1, &this->DepthRenderTarget, NULL );

	// 10000.0f is the Far Clip Plane Distance that is the farthest default depth value
	float ClearColor[ 4 ] = { farClip, 0.0f, 0.0f, 0.0f };

	d3d11DevCon.ClearRenderTargetView( this->DepthRenderTarget, ClearColor );
	d3d11DevCon.OMSetBlendState( this->ShaderManager.D3dDepthTestsBlendState.p, 0, 0xffffffff );
}
