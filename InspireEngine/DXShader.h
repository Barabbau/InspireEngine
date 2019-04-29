#pragma once

//Include and link appropriate libraries and headers//
#include <dwrite.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>

class DXShader
{
	public:
	HRESULT hr;

	ID3D11InputLayout* VertLayout;

	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;
	
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;

	DXShader( LPCSTR vsFunctionName,
			  LPCSTR psFunctionName,
			  D3D11_INPUT_ELEMENT_DESC &layout,
			  int numElements,
			  ID3D11Device &d3d11Device,
			  LPWSTR fileName = nullptr );
	~DXShader( );

	HRESULT DXShader::Compile( LPCSTR vsFunctionName,
							   LPCSTR psFunctionName,
							   ID3D11Device &d3d11Device,
							   LPWSTR fileName = nullptr );

	HRESULT DXShader::SetInputLayout( D3D11_INPUT_ELEMENT_DESC &layout,
									  int numElements,
									  ID3D11Device &d3d11Device );





	private:

};