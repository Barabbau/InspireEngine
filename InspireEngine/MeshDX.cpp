#pragma once

#include "MeshDX.h"

MeshDX::MeshDX( Mesh& originalObject3D )
{
	//ctor
	this->_originalObject3D = &originalObject3D;

	this->_bufferIndexesOriginalIds = new std::unordered_map<INT32, std::vector<std::vector<INT32>>>( );
	this->_bufferSize = new std::unordered_map<INT32, std::vector<INT32>>( );

	this->_vertexBufferArray = new std::unordered_map<INT32, std::vector<ID3D11BufferPtr>>( );
	this->_indexBufferArray = new std::unordered_map<INT32, std::vector<ID3D11BufferPtr>>( );
}


MeshDX::~MeshDX( )
{
	//meshVertBuff->Release( );
	//meshIndexBuff->Release( );
}

/// <summary>
/// Create a list of VertexBuffers and Index buffers for each material id of the Object
/// I divides the list using the max possible size for draw call efficiency
/// </summary>
/// <param name="device">DirectX 11 Device</param>
void MeshDX::CreateVertexBufferArray( ID3D11Device &d3d11Device )
{
	for ( size_t i = 0; i < this->_originalObject3D->MaterialIds.size( ); i++ )
	{
		INT32 materialId = this->_originalObject3D->MaterialIds[ i ];

		// Get all the faces using the specific material Id
		std::vector<DXFace> facesList;

		for ( size_t f = 0; f < this->_originalObject3D->Faces.size( ); f++ )
		{
			if ( this->_originalObject3D->Faces[ f ].materialId == materialId )
			{
				facesList.push_back( this->_originalObject3D->Faces[ f ] );
			}
		}

		INT32 numPrimitives = facesList.size( );
		INT32 offset = 0;
		INT32 end = 0;
		INT32 foundIndex = 0;

		while ( end < numPrimitives )
		{
			INT32 start = foundIndex;
			end = start + BUFFER_LIMIT;
			end = end > numPrimitives ? numPrimitives : end;

			foundIndex = AddVertexBuffer( d3d11Device, facesList, materialId, start, end, offset );

			offset++;
		}
	}
}

/// <summary>
/// Add a Vertex buffer to the list of Buffer associated to a specific material id
/// </summary>
/// <param name="device">Graphic Device</param>
/// <param name="faceList">list of face using the specific material id</param>
/// <param name="materialId">the material id to which we are adding a buffer</param>
/// <param name="start">the starting index of the face to add to the new buffer</param>
/// <param name="end">the ending index of the face to add to the new buffer</param>
/// <param name="bufferIndex">the buffer index</param>
/// <returns>the ending face index reached by the buffer</returns>
INT32 MeshDX::AddVertexBuffer(
	ID3D11Device &d3d11Device,
	const std::vector<DXFace> &faceList,
	INT32 materialId,
	INT32 start,
	INT32 end,
	INT32 bufferIndex )
{
	INT32 foundIndex = start;

	// --- init vertices
	std::vector<DXVertex> vertexData;

	// --- init indices
	std::vector<DWORD> indexData;

	std::vector<INT32> idsVertexesDictionary;
	std::vector<INT32> idsNormalsDictionary;
	std::vector<INT32> idsUvsDictionary;
	std::vector<INT32> idsTangentsDictionary;

	try
	{
		for ( size_t i = start; i < end; ++i )
		{
			const DXFace &face = faceList[ i ];

			for ( size_t v = 0; v < 3; ++v )
			{
				INT32 stdIndex = i * 3 + v;

				idsVertexesDictionary.push_back( face.indexes[ v ] );
				idsNormalsDictionary.push_back( face.normalIndexes[ v ] );
				idsUvsDictionary.push_back( face.uvIndexes[ v ] );
				idsTangentsDictionary.push_back( face.normalIndexes[ v ] );

				indexData.push_back( ( DWORD ) stdIndex );
			}
		}

		INT32 index = 0;

		for ( size_t i = 0; i < idsVertexesDictionary.size( ); ++i )
		{
			INT32 remappedId = idsVertexesDictionary[ i ];

			DXVertex tempVert;

			// position
			tempVert.pos = this->_originalObject3D->vertices[ remappedId ];

			// normal
			tempVert.normal = this->_originalObject3D->normals[ idsNormalsDictionary[ index ] ];

			// uv
			tempVert.texCoord = this->_originalObject3D->uvs[ idsUvsDictionary[ index ] ];

			// tangents
			tempVert.tangent = this->_originalObject3D->tangents[ idsTangentsDictionary[ index ] ];

			vertexData.push_back( tempVert );

			foundIndex++;

			index++;
		}
	}
	catch ( int e )
	{
		std::cout << "Unable to open allocate buffer. Exception Nr. " << e << '\n';

		return -1;
	}

	try
	{
		HRESULT hr = 0;

		// Vertex buffers
		ID3D11Buffer* vertexBuffer;

		//Create Vertex Buffer
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory( &vertexBufferDesc, sizeof( vertexBufferDesc ) );

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof( DXVertex ) * ( vertexData.size( )  );
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory( &vertexBufferData, sizeof( vertexBufferData ) );
		vertexBufferData.pSysMem = &vertexData[ 0 ];

		hr = d3d11Device.CreateBuffer( &vertexBufferDesc, &vertexBufferData, &vertexBuffer );

		ID3D11BufferPtr sharedVertexBufferPtr( vertexBuffer );

		// Vertex buffers & Bindings
		// search in the dictionary to see if the vertex buffer for this material id already exists
		auto got = this->_vertexBufferArray->find( materialId );
		if ( got == this->_vertexBufferArray->end( ) )
		{
			std::vector<ID3D11BufferPtr> newVertexBufferList;

			newVertexBufferList.push_back( sharedVertexBufferPtr );

			std::pair<INT32, std::vector<ID3D11BufferPtr>> myVertBuffers( materialId, newVertexBufferList );
			this->_vertexBufferArray->insert( myVertBuffers );
		}
		else
		{
			this->_vertexBufferArray->at( materialId ).push_back( sharedVertexBufferPtr );
		}

		// Index Buffer
		ID3D11Buffer* indexBuffer;

		//Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory( &indexBufferDesc, sizeof( indexBufferDesc ) );

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof( DWORD ) * indexData.size( );
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = &indexData[ 0 ];
		d3d11Device.CreateBuffer( &indexBufferDesc, &iinitData, &indexBuffer );

		ID3D11BufferPtr sharedIndexBufferPtr( indexBuffer );

		// Index Buffers
		// search in the dictionary to see if the index buffer for this material id already exists
		std::unordered_map<INT32, std::vector<ID3D11BufferPtr>>::const_iterator gotIndex = _indexBufferArray->find( materialId );
		if ( gotIndex == _indexBufferArray->end( ) )
		{
			std::vector<ID3D11BufferPtr> newIndexBufferList;

			newIndexBufferList.push_back( sharedIndexBufferPtr );

			std::pair<INT32, std::vector<ID3D11BufferPtr>> myIndexBuffers( materialId, newIndexBufferList );
			_indexBufferArray->insert( myIndexBuffers );

			// Buffer sizes
			std::vector<INT32> newBufferSizes;

			newBufferSizes.push_back( indexData.size( ) );

			std::pair<INT32, std::vector<INT32>> myBufferSizes( materialId, newBufferSizes );
			this->_bufferSize->insert( myBufferSizes );
		}
		else
		{
			this->_indexBufferArray->at( materialId ).push_back( sharedIndexBufferPtr );

			this->_bufferSize->at( materialId ).push_back( indexData.size( ) );
		}

		return foundIndex;
	}
	catch ( int e )
	{
		std::cout << "Unable to open allocate buffer. Exception Nr. " << e << '\n';

		return -1;
	}
}

/// <summary>
/// Add a Vertex buffer to the list of Buffer associated to a specific material id
/// </summary>
/// <param name="device">Graphic Device</param>
/// <param name="materialId">the material id to which we are adding a buffer</param>
/// <param name="bufferIndex">the buffer index</param>
/// <param name="myBuffer">new vert buffer</param>
/// <param name="myIndexBuffer">new index buffer</param>
/// <returns>the ending face index reached by the buffer</returns>
void MeshDX::UpdateDynamicVertexBuffer(
	ID3D11Device* d3d11Device, 
	INT32 materialId, 
	INT32 bufferIndex,
	const std::vector<DXVertex> &myBuffer,
	std::vector<UINT16> &myIndexBuffer)
{
	ID3D11DeviceContext *ppContext;

	d3d11Device->GetImmediateContext( &ppContext );

	D3D11_MAPPED_SUBRESOURCE resource;

	ppContext->Map( this->_vertexBufferArray->at( materialId )[ bufferIndex ], 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &resource );
	memcpy( resource.pData, &myBuffer, myBuffer.size() * sizeof(DXVertex) );
	ppContext->Unmap( this->_vertexBufferArray->at( materialId )[ bufferIndex ], 0 );
}


/// <summary>
/// Dispose Buffers
/// </summary>
void MeshDX::DisposeBuffers( bool cleanAll )
{
	this->_bufferSize->clear( );
	for ( int a = 0; a < this->_indexBufferArray->size( ); a++ )
	{
		for ( size_t i = 0; i < this->_indexBufferArray->at( a ).size( ); i++ )
		{
			this->_indexBufferArray->at( a ).at( i ).Release( );
		}
		this->_indexBufferArray->at( a ).clear( );
	}
	this->_indexBufferArray->clear( );

	for ( int a = 0; a < this->_vertexBufferArray->size( ); a++ )
	{
		for ( size_t i = 0; i < this->_vertexBufferArray->at( a ).size( ); i++ )
		{
			this->_vertexBufferArray->at( a ).at( i ).Release( );
		}
		this->_vertexBufferArray->at( a ).clear( );
	}
	this->_vertexBufferArray->clear( );

	this->_bufferIndexesOriginalIds->clear( );

	if ( cleanAll )
	{
		_originalObject3D->Faces.clear( );
	}
}

/// <summary>
/// Draw Standard Objects
/// </summary>
void MeshDX::DrawDepth(
	ID3D11DeviceContext &d3d11DevCon,
	DXShader &shader,
	std::vector<SurfaceMaterial> &materialsList,
	XMMATRIX viewProjection,
	DXShaderManager &shaderManager,
	XMMATRIX World )
{
	UINT stride = sizeof( DXVertex );
	UINT offset = 0;

	//Set the WVP matrix and send it to the constant buffer in effect file
	XMMATRIX WVP = World * viewProjection;

	d3d11DevCon.PSSetSamplers( 0, 1, &shaderManager.TexSamplerState.p );

	for ( size_t i = 0; i < this->_originalObject3D->MaterialIds.size( ); ++i )
	{
		INT32 materialId = this->_originalObject3D->MaterialIds[ i ];
		SurfaceMaterial* surfaceMaterial = &materialsList[ materialId ];

		if ( surfaceMaterial->transparent )
		{
			continue;
		}

		d3d11DevCon.IASetInputLayout( shader.VertLayout );

		//Set Vertex and Pixel Shaders
		d3d11DevCon.VSSetShader( shader.VS, 0, 0 );
		d3d11DevCon.PSSetShader( shader.PS, 0, 0 );

		// set per frame constant buffer
		shaderManager.cbDepthPerFrame.WorldViewProjection = XMMatrixTranspose( WVP );

		d3d11DevCon.UpdateSubresource( shaderManager.cbDepthPerFrameBuffer, 0, NULL, &shaderManager.cbDepthPerFrame, 0, 0 );
		d3d11DevCon.VSSetConstantBuffers( 0, 1, &shaderManager.cbDepthPerFrameBuffer.p );
		d3d11DevCon.PSSetConstantBuffers( 0, 1, &shaderManager.cbDepthPerFrameBuffer.p );

		for ( INT32 a = 0; a < this->_indexBufferArray->at( materialId ).size( ); a++ )
		{
			//Set the grounds index buffer
			d3d11DevCon.IASetIndexBuffer( this->_indexBufferArray->at( materialId ).at( a ), DXGI_FORMAT_R32_UINT, 0 );

			//Set the grounds vertex buffer
			d3d11DevCon.IASetVertexBuffers( 0, 1, &this->_vertexBufferArray->at( materialId ).at( a ), &stride, &offset );


			d3d11DevCon.DrawIndexed( this->_bufferSize->at( materialId ).at( a ), 0, 0 );
		}
	}
}

/// <summary>
/// Draw Standard Objects
/// </summary>
void MeshDX::Draw(
	ID3D11DeviceContext &d3d11DevCon,
	XMMATRIX viewProjection,
	std::vector<SurfaceMaterial> &materialsList,
	DXShaderManager &shaderManager,
	XMMATRIX World,
	Light &light )
{
	UINT stride = sizeof( DXVertex );
	UINT offset = 0;

	XMMATRIX meshWorld = XMMatrixIdentity( );

	//Set the WVP matrix and send it to the constant buffer in effect file
	XMMATRIX WVP = World * viewProjection;
	
	shaderManager.cbPerObj.World = XMMatrixTranspose( World );

	d3d11DevCon.PSSetSamplers( 0, 1, &shaderManager.TexSamplerState.p );

	for ( size_t i = 0; i < this->_originalObject3D->MaterialIds.size( ); ++i )
	{
		INT32 materialId = this->_originalObject3D->MaterialIds[ i ];

		SurfaceMaterial* surfaceMaterial = &materialsList[ materialId ];

		if ( surfaceMaterial->transparent )
		{
			//Set our blend state
			d3d11DevCon.OMSetBlendState( shaderManager.Transparency, NULL, 0xffffffff );
		}
		else
		{
			//Set the default blend state (no blending) for opaque objects
			d3d11DevCon.OMSetBlendState( 0, 0, 0xffffffff );
		}

		d3d11DevCon.IASetInputLayout( surfaceMaterial->shader->VertLayout );

		//Set Vertex and Pixel Shaders
		d3d11DevCon.VSSetShader( surfaceMaterial->shader->VS, 0, 0 );
		d3d11DevCon.PSSetShader( surfaceMaterial->shader->PS, 0, 0 );

		// set per frame constant buffer
		shaderManager.cbPerFrame.light = light;

		d3d11DevCon.UpdateSubresource( shaderManager.cbPerFrameBuffer, 0, NULL, &shaderManager.cbPerFrame, 0, 0 );
		d3d11DevCon.VSSetConstantBuffers( 0, 1, &shaderManager.cbPerFrameBuffer.p );
		d3d11DevCon.PSSetConstantBuffers( 0, 1, &shaderManager.cbPerFrameBuffer.p );

		// set per object constant buffer
		shaderManager.cbPerObj.WVP = XMMatrixTranspose( WVP );

		shaderManager.cbPerObj.difColor = surfaceMaterial->difColor;
		shaderManager.cbPerObj.hasTexture = surfaceMaterial->hasTexture;

		d3d11DevCon.UpdateSubresource( shaderManager.cbPerObjectBuffer, 0, NULL, &shaderManager.cbPerObj, 0, 0 );
		d3d11DevCon.VSSetConstantBuffers( 1, 1, &shaderManager.cbPerObjectBuffer.p );
		d3d11DevCon.PSSetConstantBuffers( 1, 1, &shaderManager.cbPerObjectBuffer.p );

		if ( surfaceMaterial->hasTexture )
		{
			d3d11DevCon.PSSetShaderResources( 0, 1, &surfaceMaterial->albedoTexture );
			d3d11DevCon.PSSetShaderResources( 1, 1, &surfaceMaterial->normalTexture );
			d3d11DevCon.PSSetShaderResources( 2, 1, &shaderManager.skyboxTexture );
		}

		for ( INT32 a = 0; a < this->_indexBufferArray->at( materialId ).size( ); a++ )
		{
			//Set the grounds index buffer
			d3d11DevCon.IASetIndexBuffer( this->_indexBufferArray->at( materialId ).at( a ), DXGI_FORMAT_R32_UINT, 0 );

			//Set the grounds vertex buffer
			d3d11DevCon.IASetVertexBuffers( 0, 1, &this->_vertexBufferArray->at( materialId ).at( a ), &stride, &offset );

			d3d11DevCon.DrawIndexed( this->_bufferSize->at( materialId ).at( a ), 0, 0 );
		}
	}
}

/// <summary>
/// Draw Instanced Indexed
/// </summary>
/// <param name="device"></param>
/// <param name="baseCamera"></param>
/// <param name="materialDictionary"></param>
/// <param name="shaderManager"></param>
/// <param name="blendManager"></param>
/// <param name="matrices">the transforms, the last call will still use the max number of transforms</param>
/// <param name="instanceCount">the amount of instances, the last call must use only the number to be drawn</param>
void MeshDX::DrawInstanced(
	ID3D11DeviceContext &d3d11DevCon,
	XMMATRIX viewProjection,
	std::vector<SurfaceMaterial> &materialsList,
	DXShaderManager &shaderManager,
	XMMATRIX World[ ],
	INT32 instanceCount,
	Light &light )
{
	UINT stride = sizeof( DXVertex );
	UINT offset = 0;

	XMMATRIX meshWorld = XMMatrixIdentity( );
	//Set the WVP matrix and send it to the constant buffer in effect file

	XMMATRIX WVP = meshWorld * viewProjection;

	//XMMATRIX tempMatrix;
	for ( int i = 0; i < 512; i++ )
	{
		// To make things simple, we just store the matrix directly into our cbPerInst structure
		shaderManager.cbPerObjInstanced.World[ i ] = World[ i ];
	}

	d3d11DevCon.PSSetSamplers( 0, 1, &shaderManager.TexSamplerState.p );


	for ( size_t i = 0; i < this->_originalObject3D->MaterialIds.size( ); ++i )
	{
		INT32 materialId = this->_originalObject3D->MaterialIds[ i ];

		SurfaceMaterial* surfaceMaterial = &materialsList[ materialId ];

		if ( surfaceMaterial->transparent )
		{
			//Set our blend state
			d3d11DevCon.OMSetBlendState( shaderManager.Transparency, NULL, 0xffffffff );
		}
		else
		{
			//Set the default blend state (no blending) for opaque objects
			d3d11DevCon.OMSetBlendState( 0, 0, 0xffffffff );
		}

		d3d11DevCon.IASetInputLayout( surfaceMaterial->shader->VertLayout );

		//Set Vertex and Pixel Shaders
		d3d11DevCon.VSSetShader( surfaceMaterial->shader->VS, 0, 0 );
		d3d11DevCon.PSSetShader( surfaceMaterial->shader->PS, 0, 0 );

		// set per frame constant buffer
		shaderManager.cbPerFrame.light = light;

		d3d11DevCon.UpdateSubresource( shaderManager.cbPerFrameBuffer, 0, NULL, &shaderManager.cbPerFrame, 0, 0 );
		d3d11DevCon.VSSetConstantBuffers( 0, 1, &shaderManager.cbPerFrameBuffer.p );
		d3d11DevCon.PSSetConstantBuffers( 0, 1, &shaderManager.cbPerFrameBuffer.p );

		// set per object constant buffer
		shaderManager.cbPerObjInstanced.WVP = XMMatrixTranspose( WVP );

		shaderManager.cbPerObjInstanced.difColorInstanced = surfaceMaterial->difColor;
		shaderManager.cbPerObjInstanced.hasTextureInstanced = surfaceMaterial->hasTexture;

		d3d11DevCon.UpdateSubresource( shaderManager.cbPerObjectBufferInstanced, 0, NULL, &shaderManager.cbPerObjInstanced, 0, 0 );
		d3d11DevCon.VSSetConstantBuffers( 2, 1, &shaderManager.cbPerObjectBufferInstanced.p );
		d3d11DevCon.PSSetConstantBuffers( 2, 1, &shaderManager.cbPerObjectBufferInstanced.p );

		if ( surfaceMaterial->hasTexture )
		{
			d3d11DevCon.PSSetShaderResources( 0, 1, &surfaceMaterial->albedoTexture );
			d3d11DevCon.PSSetShaderResources( 1, 1, &surfaceMaterial->normalTexture );
			d3d11DevCon.PSSetShaderResources( 2, 1, &shaderManager.skyboxTexture );
		}
		 
		for ( INT32 a = 0; a < this->_indexBufferArray->at( materialId ).size( ); a++ )
		{
			//Set the grounds index buffer
			d3d11DevCon.IASetIndexBuffer( this->_indexBufferArray->at( materialId )[ a ], DXGI_FORMAT_R32_UINT, 0 );

			//Set the grounds vertex buffer
			d3d11DevCon.IASetVertexBuffers( 0, 1, &this->_vertexBufferArray->at( materialId )[ a ], &stride, &offset );

			d3d11DevCon.DrawIndexedInstanced( _bufferSize->at( materialId )[ a ], instanceCount, 0, 0, 0 );
		}
	}
}



