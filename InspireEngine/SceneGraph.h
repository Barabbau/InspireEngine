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
#include "EditorMeshInstanced.h"
#include "EditorMesh.h"

class SceneGraph
{
	public:
	typedef std::shared_ptr<EditorMesh> EditorMeshPtr;
	typedef std::shared_ptr<EditorMeshInstanced> EditorMeshInstancedPtr;
	SceneGraph(
		ID3D11DeviceContext &d3d11DevCon,
		ID3D11Device &d3d11Device,
		std::string fileName,
		std::vector<SurfaceMaterial> &materialsList,
		std::vector<EditorMeshPtr> &lstEditorObject3Ds,
		Camera &camera,
		DXShaderManager &shaderManager,
		Light &light,
		InspireUtils &inspireUtils );
	~SceneGraph( );

	bool SceneRead(
		ID3D11DeviceContext &d3d11DevCon,
		ID3D11Device &d3d11Device,
		std::string fileName,
		std::vector<SurfaceMaterial> &materialsList,
		std::vector<EditorMeshPtr> &lstEditorObject3Ds,
		Camera &camera,
		DXShaderManager &shaderManager,
		Light &light,
		InspireUtils &inspireUtils );

	void Render( XMMATRIX ViewPerspective );

	XMFLOAT3 RootPosition;
	XMFLOAT3 RootRotation;
	XMFLOAT3 RootScale;

	std::unordered_map<std::string, EditorMeshInstancedPtr> *InstancedObjects;

	private:

};
