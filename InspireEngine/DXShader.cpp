#pragma once

#include "DXShader.h"

DXShader::DXShader( LPCSTR vsFunctionName,
					LPCSTR psFunctionName,
					D3D11_INPUT_ELEMENT_DESC &layout,
					int numElements,
					ID3D11Device &d3d11Device )
{
	// ctor
	// Compile Shaders from shader file
	this->hr = this->Compile( vsFunctionName, psFunctionName, d3d11Device );

	// Create the Input Layout
	this->hr = this->SetInputLayout( layout, numElements, d3d11Device );
}

DXShader::~DXShader( )
{
	/*
	this->VS->Release( );
	this->PS->Release( );
	this->VS_Buffer->Release( );
	this->PS_Buffer->Release( );
	this->VertLayout->Release( );
	*/
}

HRESULT DXShader::Compile( LPCSTR vsFunctionName,
						   LPCSTR psFunctionName,
						   ID3D11Device &d3d11Device )
{
	//Compile Shaders from shader file
	if ( vsFunctionName != nullptr )
	{
		hr = D3DX11CompileFromFile( L"Effects.fx", 0, 0, vsFunctionName, "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0 );
	}

	if ( psFunctionName != nullptr )
	{
		hr = D3DX11CompileFromFile( L"Effects.fx", 0, 0, psFunctionName, "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0 );
	}

	//Create the Shader Objects
	hr = d3d11Device.CreateVertexShader( VS_Buffer->GetBufferPointer( ), VS_Buffer->GetBufferSize( ), NULL, &VS );
	hr = d3d11Device.CreatePixelShader( PS_Buffer->GetBufferPointer( ), PS_Buffer->GetBufferSize( ), NULL, &PS );

	return hr;
}

HRESULT DXShader::SetInputLayout( D3D11_INPUT_ELEMENT_DESC &layout,
								  int numElements,
								  ID3D11Device &d3d11Device )
{
	//Create the Input Layout
	hr = d3d11Device.CreateInputLayout( &layout, 
									 numElements, 
									 VS_Buffer->GetBufferPointer(),									 
									 VS_Buffer->GetBufferSize(), 
									 &VertLayout );

	return hr;
}