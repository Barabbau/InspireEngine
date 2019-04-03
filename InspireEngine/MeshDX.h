#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <iostream>
#include <unordered_map>
#include <atlbase.h>

#include "Mesh.h"
#include "Camera.h"
#include "DXShaderManager.h"
#include "SurfaceMaterial.h"
#include "Light.h"


class MeshDX
{
	public:
	MeshDX( Mesh& originalObject3D );//constructor
	~MeshDX( );

	typedef CComPtr<ID3D11Buffer> ID3D11BufferPtr;

	void MeshDX::CreateVertexBufferArray( ID3D11Device &d3d11Device );
	void MeshDX::DisposeBuffers( bool cleanAll = true );

	INT32 MeshDX::AddVertexBuffer(
		ID3D11Device  &d3d11Device,
		const std::vector<DXFace> &faceList,
		INT32 materialId,
		INT32 start,
		INT32 end,
		INT32 bufferIndex );

	void MeshDX::UpdateDynamicVertexBuffer(
		ID3D11Device* d3d11Device,
		INT32 materialId,
		INT32 bufferIndex,
		const std::vector<DXVertex> &myBuffer,
		std::vector<UINT16> &myIndexBuffer );

	void MeshDX::Draw(
		ID3D11DeviceContext &d3d11DevCon,
		XMMATRIX viewProjection,
		std::vector<SurfaceMaterial> &materialsList,
		DXShaderManager &shaderManager,
		XMFLOAT3 rot,
		XMFLOAT3 pos,
		XMFLOAT3 scale,
		Light &light );

	void MeshDX::DrawInstanced(
		ID3D11DeviceContext &d3d11DevCon,
		XMMATRIX viewProjection,
		std::vector<SurfaceMaterial> &materialsList,
		DXShaderManager &shaderManager,
		std::vector<XMMATRIX> matrices,
		INT32 instanceCount,
		Light &light );

	const INT32 BUFFER_LIMIT = 65535;
	D3DXCOLOR _defaultColor = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	Mesh* _originalObject3D;

	/// <summary>
	/// A Dictionary to map the reorganized vertex ids to reuse verts 
	/// </summary>
	std::unordered_map<INT32, std::vector<std::vector<INT32>>> *_bufferIndexesOriginalIds;

	/// <summary>
	/// A Dictionary of Lists of pre calculated sizes for each buffers
	/// needed to pass the value to the shaders
	/// </summary>
	std::unordered_map<INT32, std::vector<INT32>> *_bufferSize;

	/// <summary>
	/// D3D Buffer
	/// Dictionary of Lists of Vertex Buffers
	/// Every material id has multiple vertex buffer filled to the vertex buffer size limit
	/// Assuming that we use triangles the vertex buffer limit is 65535;
	/// </summary>
	std::unordered_map<INT32, std::vector<ID3D11BufferPtr>> *_vertexBufferArray;

	/// <summary>
	/// The Dictionary of List of Buffer Indexes
	/// The Editor reorders the indexes in a sequential format
	/// </summary>
	std::unordered_map<INT32, std::vector<ID3D11BufferPtr>> *_indexBufferArray;
};
