#include "SkySphere.h"


SkySphere::SkySphere( ID3D11Device &d3d11Device, int LatLines, int LongLines )
{
	NumSphereVertices = ( ( LatLines - 2 ) * LongLines ) + 2;
	NumSphereFaces = ( ( LatLines - 3 )*( LongLines ) * 2 ) + ( LongLines * 2 );

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<DXVertex> vertices( NumSphereVertices );

	XMVECTOR currVertPos = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );

	vertices[ 0 ].pos.x = 0.0f;
	vertices[ 0 ].pos.y = 0.0f;
	vertices[ 0 ].pos.z = 1.0f;

	for ( DWORD i = 0; i < LatLines - 2; ++i )
	{
		spherePitch = ( i + 1 ) * ( 3.14f / ( LatLines - 1 ) );
		Rotationx = XMMatrixRotationX( spherePitch );
		for ( DWORD j = 0; j < LongLines; ++j )
		{
			sphereYaw = j * ( 6.28f / ( LongLines ) );
			Rotationy = XMMatrixRotationZ( sphereYaw );
			currVertPos = XMVector3TransformNormal( XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ), ( Rotationx * Rotationy ) );
			currVertPos = XMVector3Normalize( currVertPos );
			vertices[ i*LongLines + j + 1 ].pos.x = XMVectorGetX( currVertPos );
			vertices[ i*LongLines + j + 1 ].pos.y = XMVectorGetY( currVertPos );
			vertices[ i*LongLines + j + 1 ].pos.z = XMVectorGetZ( currVertPos );
		}
	}

	vertices[ NumSphereVertices - 1 ].pos.x = 0.0f;
	vertices[ NumSphereVertices - 1 ].pos.y = 0.0f;
	vertices[ NumSphereVertices - 1 ].pos.z = -1.0f;

	ID3D11Buffer* vertexBuffer;

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof( vertexBufferDesc ) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( DXVertex ) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory( &vertexBufferData, sizeof( vertexBufferData ) );
	vertexBufferData.pSysMem = &vertices[ 0 ];
	HRESULT hr = d3d11Device.CreateBuffer( &vertexBufferDesc, &vertexBufferData, &vertexBuffer );

	hr = d3d11Device.CreateBuffer( &vertexBufferDesc, &vertexBufferData, &vertexBuffer );
	ID3D11BufferPtr sharedVertexBufferPtr( vertexBuffer );
	this->VertexBuffer = sharedVertexBufferPtr;

	std::vector<DWORD> indices( NumSphereFaces * 3 );

	int k = 0;
	for ( DWORD l = 0; l < LongLines - 1; ++l )
	{
		indices[ k ] = 0;
		indices[ k + 1 ] = l + 1;
		indices[ k + 2 ] = l + 2;
		k += 3;
	}

	indices[ k ] = 0;
	indices[ k + 1 ] = LongLines;
	indices[ k + 2 ] = 1;
	k += 3;

	for ( DWORD i = 0; i < LatLines - 3; ++i )
	{
		for ( DWORD j = 0; j < LongLines - 1; ++j )
		{
			indices[ k ] = i * LongLines + j + 1;
			indices[ k + 1 ] = i * LongLines + j + 2;
			indices[ k + 2 ] = ( i + 1 )*LongLines + j + 1;

			indices[ k + 3 ] = ( i + 1 )*LongLines + j + 1;
			indices[ k + 4 ] = i * LongLines + j + 2;
			indices[ k + 5 ] = ( i + 1 )*LongLines + j + 2;

			k += 6; // next quad
		}

		indices[ k ] = ( i*LongLines ) + LongLines;
		indices[ k + 1 ] = ( i*LongLines ) + 1;
		indices[ k + 2 ] = ( ( i + 1 )*LongLines ) + LongLines;

		indices[ k + 3 ] = ( ( i + 1 )*LongLines ) + LongLines;
		indices[ k + 4 ] = ( i*LongLines ) + 1;
		indices[ k + 5 ] = ( ( i + 1 )*LongLines ) + 1;

		k += 6;
	}

	for ( DWORD l = 0; l < LongLines - 1; ++l )
	{
		indices[ k ] = NumSphereVertices - 1;
		indices[ k + 1 ] = ( NumSphereVertices - 1 ) - ( l + 1 );
		indices[ k + 2 ] = ( NumSphereVertices - 1 ) - ( l + 2 );
		k += 3;
	}

	indices[ k ] = NumSphereVertices - 1;
	indices[ k + 1 ] = ( NumSphereVertices - 1 ) - LongLines;
	indices[ k + 2 ] = NumSphereVertices - 2;

	ID3D11Buffer* indexBuffer;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof( indexBufferDesc ) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( DWORD ) * NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[ 0 ];
	hr = d3d11Device.CreateBuffer( &indexBufferDesc, &iinitData, &indexBuffer );
	ID3D11BufferPtr sharedIndexBufferPtr( indexBuffer );
	this->IndexBuffer = sharedIndexBufferPtr;
}

SkySphere::~SkySphere( )
{
	this->IndexBuffer.Release( );
	this->VertexBuffer.Release( );
}

void SkySphere::Draw(
	ID3D11DeviceContext &d3d11DevCon,
	DXShaderManager &shaderManager,
	Camera &camera )
{
	UINT stride = sizeof( DXVertex );
	UINT offset = 0;

	//Set the spheres index buffer
	d3d11DevCon.IASetIndexBuffer( this->IndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	//Set the spheres vertex buffer
	d3d11DevCon.IASetVertexBuffers( 0, 1, &this->VertexBuffer, &stride, &offset );

	//Set the WVP matrix and send it to the constant buffer in effect file
	XMMATRIX WVP = World * camera.CamView * camera.CamProjection;
	shaderManager.cbPerObj.WVP = XMMatrixTranspose( WVP );
	shaderManager.cbPerObj.World = XMMatrixTranspose( World );

	d3d11DevCon.UpdateSubresource( shaderManager.cbPerObjectBuffer, 0, NULL, &shaderManager.cbPerObj, 0, 0 );
	d3d11DevCon.VSSetConstantBuffers( 1, 1, &shaderManager.cbPerObjectBuffer.p );

	//Send our skymap resource view to pixel shader
	d3d11DevCon.PSSetShaderResources( 0, 1, &shaderManager.skyboxTexture );
	d3d11DevCon.PSSetSamplers( 0, 1, &shaderManager.TexSamplerState.p );

	//Set the new VS and PS shaders
	d3d11DevCon.VSSetShader( shaderManager._flatColorShader->VS, 0, 0 );
	d3d11DevCon.PSSetShader( shaderManager._flatColorShader->PS, 0, 0 );

	//Set the new depth/stencil and RS states
	d3d11DevCon.OMSetDepthStencilState( shaderManager.DSLessEqual, 0 );
	d3d11DevCon.RSSetState( shaderManager.RSCullNone );
	d3d11DevCon.DrawIndexed( NumSphereFaces * 3, 0, 0 );
}

void SkySphere::DrawDepth(
	ID3D11DeviceContext &d3d11DevCon,
	DXShaderManager &shaderManager,
	DXShader &shader,
	Camera &camera )
{
	UINT stride = sizeof( DXVertex );
	UINT offset = 0;

	//Set the spheres index buffer
	d3d11DevCon.IASetIndexBuffer( this->IndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	//Set the spheres vertex buffer
	d3d11DevCon.IASetVertexBuffers( 0, 1, &this->VertexBuffer, &stride, &offset );

	//Set the WVP matrix and send it to the constant buffer in effect file
	XMMATRIX WVP = World * camera.CamView * camera.CamProjection;

	// set per frame constant buffer
	shaderManager.cbDepthPerFrame.WorldViewProjection = XMMatrixTranspose( WVP );

	d3d11DevCon.UpdateSubresource( shaderManager.cbDepthPerFrameBuffer, 0, NULL, &shaderManager.cbDepthPerFrame, 0, 0 );
	d3d11DevCon.VSSetConstantBuffers( 0, 1, &shaderManager.cbDepthPerFrameBuffer.p );
	d3d11DevCon.PSSetConstantBuffers( 0, 1, &shaderManager.cbDepthPerFrameBuffer.p );

	//Set Vertex and Pixel Shaders
	d3d11DevCon.VSSetShader( shader.VS, 0, 0 );
	d3d11DevCon.PSSetShader( shader.PS, 0, 0 );

	//Set the new depth/stencil and RS states
	//d3d11DevCon.OMSetDepthStencilState( shaderManager.DSLessEqual, 0 );
	d3d11DevCon.RSSetState( shaderManager.RSCullNone );
	d3d11DevCon.DrawIndexed( NumSphereFaces * 3, 0, 0 );
	//	d3d11DevCon.OMSetDepthStencilState( NULL, 0 );
}