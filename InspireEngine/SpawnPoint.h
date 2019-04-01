#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

class SpawnPoint
{
	public:
	SpawnPoint( );
	~SpawnPoint( );

	INT32 type;

	XMFLOAT3 rotation;
	XMFLOAT3 position;
	XMFLOAT3 scale;

	private:

};
