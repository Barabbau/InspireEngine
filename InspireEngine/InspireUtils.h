#pragma once

#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cstdlib>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <iostream>
#include <unordered_map>

#include "Mesh.h"
#include "Camera.h"
#include "DXShaderManager.h"
#include "SurfaceMaterial.h"
#include "Light.h"



class InspireUtils
{
	public:
	InspireUtils( );
	~InspireUtils( );

	bool InspireUtils::ObjRead(
		ID3D11Device &d3d11Device,
		DXShaderManager &shaderManager,
		std::string fileName,
		std::string pathModels,
		std::vector<SurfaceMaterial> &materialsList,
		Mesh* object3D,
		XMFLOAT3 &minVertex,
		XMFLOAT3 &maxVertex,
		INT32 materialType );

	bool InspireUtils::ReadMaterialIdName(
		ID3D11Device &d3d11Device,
		Mesh* object3D,
		const std::string &pathModels,
		const std::string &line,
		INT32 materialId,
		std::vector<SurfaceMaterial> &materialsList,
		DXShaderManager &shaderManager,
		INT32 materialType = 1 );



	std::vector<std::string> InspireUtils::split( const std::string &s, char delim );
	int InspireUtils::StringToWString( std::wstring &ws, const std::string &s );
	char InspireUtils::to_lowercase( char c );

	/// <summary>
	/// The current material id used for render & sorting purposes
	/// </summary>
	INT32 CurrentId;


	private:

};
