#pragma once

#include <vector>
#include <string>

#include "DXShader.h"

//Create material structure
class SurfaceMaterial
{
	public:
	SurfaceMaterial( );
	~SurfaceMaterial( );

	char *matName;
	XMFLOAT4 difColor;

	bool hasTexture;
	bool transparent;

	DXShader* shader;
	ID3D11ShaderResourceView* albedoTexture;

	private:

};
