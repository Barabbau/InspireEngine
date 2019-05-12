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
#include "InspireUtils.h"



class SsaoManager
{
	/// Vertex Structure for Full screen Quad Sprite
	struct SsaoVertex
	{
		XMFLOAT3		Position;
	};

	/// Updating Parameters for Effect_ComputeSSAOMap.fx
	struct ScreenSSAOComputeCBChangesEveryFrame
	{
		XMFLOAT4X4		worldMatrix;
		XMFLOAT4		sampleRadius;
		XMFLOAT4		distanceScale;
	};

	public:
	SsaoManager(
		ID3D11Device  &d3d11Device,
		ID3D11DeviceContext &d3d11DevCon,
		LPCWSTR shaderFileName,
		DXShaderManager &shaderManager,
		float width,
		float height );
	~SsaoManager( );

	typedef CComPtr<ID3D11Buffer> ID3D11BufferPtr;

	void Draw( 
		ID3D11Device  &d3d11Device,
		ID3D11DeviceContext &d3d11DevCon,
		float nearClip,
		float farClip,
		float width,
		float height );

	HRESULT Intialize(
		ID3D11Device  &d3d11Device,
		ID3D11DeviceContext &d3d11DevCon,
		int numElements,
		LPCWSTR shaderFileName,
		float width,
		float height );

	HRESULT UpdateResources(
		ID3D11Device &d3d11Device,
		float width,
		float height );
	
	void SetupDepthRenderTarget(
			ID3D11Device  &d3d11Device,
			ID3D11DeviceContext &d3d11DevCon,
			float farClip );

	// Full Screen Quad Resources
	ID3D11BufferPtr			ScreenIndexBuffer = nullptr;
	ID3D11BufferPtr			ScreenVertexBuffer = nullptr;
	
	ID3D11BufferPtr         SsaoCBChangesEveryFrame = nullptr;

	DXShader* SsaoDisplayShader;
	DXShader* SsaoCreateShader;

	DXShaderManager &ShaderManager;

	// SSAO Resources
	ID3D11Texture2D*			DepthTexture = nullptr;
	ID3D11RenderTargetView*		DepthRenderTarget = nullptr;
	ID3D11ShaderResourceView*	DepthShaderResource = nullptr;

	// The Random Normal for SSAO computation
	ID3D11ShaderResourceView*	TexRandomNormal = nullptr;

	private:

};
