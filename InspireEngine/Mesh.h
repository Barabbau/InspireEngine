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

struct DXVertex	//Overloaded Vertex Structure
{
	DXVertex( )
	{
	}
	DXVertex( float x, float y, float z,
			float u, float v,
			float nx, float ny, float nz )
		: pos( x, y, z ), texCoord( u, v ), normal( nx, ny, nz )
	{
	}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

struct DXFace
{
	DXFace( )
	{
	}
	DXFace( INT32 a, INT32 b, INT32 c,
			INT32 na, INT32 nb, INT32 nc,
			INT32 ua, INT32 ub, INT32 uc,
			INT32 matId )
		: materialId( matId ), indexes(), normalIndexes( ), uvIndexes( )
	{
		indexes[ 0 ] = a;
		indexes[ 1 ] = b;
		indexes[ 2 ] = c;

		normalIndexes[ 0 ] = na;
		normalIndexes[ 1 ] = nb;
		normalIndexes[ 2 ] = nc;

		uvIndexes[ 0 ] = ua;
		uvIndexes[ 1 ] = ub;
		uvIndexes[ 2 ] = uc;
	}

	INT32 materialId;
	INT32 indexes[ 3 ];
	INT32 normalIndexes[ 3 ];
	INT32 uvIndexes[ 3 ];
};

class Mesh
{
	public:
	Mesh( );//constructor
	~Mesh( );

	bool ComputeNormals( );

	std::vector<DXFace> Faces;//to store vertex information of 3D model started with v
	std::vector<INT32> MaterialIds;//to store all 3D model face vertices
	std::vector<XMFLOAT3> vertices;//to store vertex information of 3D model started with v
	std::vector<XMFLOAT3> normals;//to store vertex information of 3D model started with v
	std::vector<XMFLOAT2> uvs;//to store vertex information of 3D model started with v
	private:
};