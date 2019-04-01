#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include "Camera.h"
#include "EditorMesh.h"
#include "SurfaceMaterial.h"
#include "DXShaderManager.h"
#include "SpawnPoint.h"
#include "Light.h"
#include "InspireUtils.h"

class EditorMeshInstanced : public EditorMesh
{
	public:

	typedef std::shared_ptr<SpawnPoint> SpawnPointPtr;
	typedef std::shared_ptr<EditorMesh> EditorMeshPtr;

	EditorMeshInstanced(
		ID3D11DeviceContext &d3d11DevCon,
		ID3D11Device &d3d11Device,
		std::string fileName,
		std::string pathModels,
		XMFLOAT3 rot,
		XMFLOAT3 pos,
		XMFLOAT3 scale,
		bool useBboxLod,
		std::vector<SurfaceMaterial> &materialsList,
		std::vector<EditorMeshPtr> &lstEditorObject3Ds,
		Camera camera,
		DXShaderManager &_shaderManager,
		Light* light,
		InspireUtils* inspireUtils
	);
	~EditorMeshInstanced( );

	void CleanTransforms( );
	void CreateTransforms( );
	void RenderInstanced( );
	void Render( INT32 index, INT32 amount );

	const INT32 INSTANCE_LIMIT = 512;
	INT32 _lastBatchSize;

	Camera _camera;
	ID3D11Device &_d3d11Device;
	ID3D11DeviceContext &_d3d11DevCon;

	std::vector<EditorMeshPtr> &_lstEditorObject3Ds;
	std::vector<SurfaceMaterial> &_materialsList;
	DXShaderManager &_shaderManager;

	std::vector<std::vector<XMMATRIX>> *_transforms;

	std::vector<SpawnPointPtr> *_spawnPoints;

	Light* _light;


	std::vector<XMMATRIX> listM;

	private:

};

