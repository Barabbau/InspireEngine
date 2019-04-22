#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include "Mesh.h"
#include "MeshDX.h"
#include "InspireUtils.h"

class EditorMesh
{
	public:
	typedef std::shared_ptr<EditorMesh> EditorMeshPtr;

	EditorMesh( );
	
	EditorMesh( ID3D11DeviceContext &d3d11DevCon,
				ID3D11Device &d3d11Device,
				std::string fileName,
				std::string pathModels,
				XMFLOAT3 rot,
				XMFLOAT3 pos,
				XMFLOAT3 scale,
				bool useBboxLod,
				std::vector<SurfaceMaterial> &materialsList,
				std::vector<EditorMeshPtr> &lstEditorObject3Ds,
				DXShaderManager &shaderManager,
				Light &light,
				InspireUtils &inspireUtils,
				INT32 materialType,
				EditorMeshPtr BBox = nullptr,
				XMCOLOR color = XMCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	EditorMesh(	ID3D11DeviceContext &d3d11DevCon,
				ID3D11Device &d3d11Device,
				std::string objectName,
				std::string pathModels,
				std::string albedoName,
				std::vector<DXVertex> &v,
				std::vector<DWORD> &indices,
				XMFLOAT3 rot,
				XMFLOAT3 pos,
				XMFLOAT3 scale,
				bool useBboxLod,
				std::vector<SurfaceMaterial> &materialsList,
				std::vector<EditorMeshPtr> &lstEditorObject3Ds,
				DXShaderManager &shaderManager,
				Light &light,
				InspireUtils &inspireUtils,
				INT32 materialType,
				EditorMeshPtr BBoxnullptr,
				XMCOLOR color = XMCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	void Render(
				ID3D11DeviceContext &d3d11DevCon,
				XMMATRIX viewProjection,
				std::vector<SurfaceMaterial> &materialsList,
				std::vector<EditorMeshPtr> &lstEditorObject3Ds,
				DXShaderManager &shaderManager,
				Light &light,
				INT32 lodIndex );

	void EditorMesh::RenderObject(
				ID3D11DeviceContext &d3d11DevCon,
				XMMATRIX viewProjection,
				std::vector<SurfaceMaterial> &materialsList,
				std::vector<EditorMeshPtr> &lstEditorObject3Ds,
				DXShaderManager &shaderManager,
				XMVECTOR cameraPosition,
		XMVECTOR cameraForward,
				Light &light );

	~EditorMesh( );

	/// <summary>
	/// Returns true if the Object has Normals
	/// </summary>
	bool HasNormal;
	bool useBboxLod;

	std::string FilePath;

	/// <summary>
	/// The id of the Object to render in the Objects list
	/// This id is used when the Object is an instance
	/// </summary>
	INT32 InstanceId;

	/// <summary>
	/// Returns true if the Object is an LOD Object
	/// </summary>
	bool IsLod;

	/// <summary>
	/// The id of the LOD Object to render in the Objects list
	/// This id is used when the Object is an instance
	/// The LOD used for render is the LOD of the original instance
	/// </summary>
	INT32 LodId = -1;

	/// <summary>
	/// The distance from the Camera
	/// Available only after culled object list is created
	/// </summary>
	INT32 ProjectedDistanceFromCamera;

	XMMATRIX Transform;


	INT32 lodSegments[ 3 ] = { 50, 1000, 10000 };

	/// <summary>
	/// The source mesh data
	/// </summary>
	Mesh _mesh;

	/// <summary>
	/// The DirectX drawable data
	/// </summary>
	MeshDX* _meshDx;

	/// <summary>
	/// The Bounding Box of the Editor Mesh
	/// </summary>
	EditorMeshPtr BBox;
	XMVECTOR minPoint;
	XMVECTOR maxPoint;

	private:

};
