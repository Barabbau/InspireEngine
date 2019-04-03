#pragma once

#include "EditorMeshInstanced.h"

EditorMeshInstanced::EditorMeshInstanced( 
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
	Camera &camera, 
	DXShaderManager &shaderManager,
	Light &light,
	InspireUtils &inspireUtils
) : _d3d11DevCon( d3d11DevCon ), _d3d11Device( d3d11Device ), _materialsList( materialsList ), _lstEditorObject3Ds( lstEditorObject3Ds ), _camera( camera ), _shaderManager( shaderManager ), _light( light ),
EditorMesh( d3d11DevCon, d3d11Device, fileName, pathModels, rot, pos, scale, useBboxLod, materialsList, lstEditorObject3Ds, shaderManager, light, inspireUtils )
{
	this->_spawnPoints = new std::vector<SpawnPointPtr>( );

	SpawnPoint* p = new SpawnPoint( );
	p->position = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	p->rotation = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	p->scale = XMFLOAT3( 1.0f, 1.0f, 1.0f );

	SpawnPointPtr firstPoint( p );
	this->_spawnPoints->push_back( firstPoint );

	this->_transforms = new std::vector<std::vector<XMMATRIX>>( );

	this->CreateTransforms( );


	for ( size_t i = 0; i < 512; i++ )
	{
		listM.push_back( XMMatrixTranspose( XMMatrixIdentity( ) ) );
	}
}

EditorMeshInstanced::~EditorMeshInstanced( )
{
}


void EditorMeshInstanced::CleanTransforms( )
{
	for ( size_t a = 0; a < this->_transforms->size(); ++a )
	{
		this->_transforms[ a ].clear( );
	}

	this->_transforms->clear( );
}


void EditorMeshInstanced::CreateTransforms( )
{
	std::vector<XMMATRIX> transforms;
	this->_transforms->push_back( transforms );

	for ( size_t i = 0; i < this->_spawnPoints->size( ); i++ )
	{
		SpawnPoint* spawnPoint = this->_spawnPoints->at( i ).get();

		XMMATRIX meshWorld = XMMatrixIdentity( );

		//Define world space matrix
		XMMATRIX Rotation = XMMatrixRotationY( spawnPoint->rotation.y );
		XMMATRIX Scale = XMMatrixScaling( spawnPoint->scale.x, spawnPoint->scale.y, spawnPoint->scale.z );
		XMMATRIX Translation = XMMatrixTranslation( spawnPoint->position.x, spawnPoint->position.y, spawnPoint->position.z );

		meshWorld = Rotation * Scale * Translation;

		meshWorld = XMMatrixTranspose( meshWorld );

		// Create new list of transforms if we max out the size of the actual one
		if ( this->_transforms[ this->_transforms->size( ) - 1 ].size( ) >= INSTANCE_LIMIT )
		{
			std::vector<XMMATRIX> newTransforms;
			this->_transforms->push_back( newTransforms );
		}

		// Add the transform to the current list
		this->_transforms->at( this->_transforms->size( ) - 1 ).push_back( meshWorld );
	}

	_lastBatchSize = this->_transforms[ this->_transforms->size( ) - 1 ].size( );

	// Pre transpose for Shader usage
	INT32 count = this->_transforms[ this->_transforms->size( ) - 1 ].size( );
	INT32 i = 0;

	while ( i < INSTANCE_LIMIT - 1 )
	{
		this->_transforms->at( this->_transforms->size( ) - 1 ).push_back( XMMatrixTranspose( XMMatrixIdentity( ) ) );
		i++;
	}
}

void EditorMeshInstanced::RenderInstanced( XMMATRIX viewProjection )
{
	for ( INT32 i = 0; i < this->_transforms->size( ) - 1; ++i )
	{
		this->Render( viewProjection, i, INSTANCE_LIMIT );
	}

	this->Render( viewProjection, this->_transforms->size( ), _lastBatchSize );
}

/// <summary>
/// Calculate the dot (scalar) product of two vectors
/// </summary>
/// <param name="left">First operand</param>
/// <param name="right">Second operand</param>
/// <returns>The dot product of the two inputs</returns>
float Dot( const XMFLOAT3 &left, const XMFLOAT3 &right )
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

void EditorMeshInstanced::Render( XMMATRIX viewProjection, INT32 index, INT32 amount )
{
	/*
	INT32 distanceToTest = 100000;
	
	if ( this->LodId != -1 )
	{
		for ( INT32 i = 0; i < this->_transforms[ index ].size( ) - 1; ++i )
		{
			//XMMATRIX transform = this->_transforms->at( index ).at( i );

			XMFLOAT3 objPos = XMFLOAT3(
//				transform->m[ 0 ][ 3 ],
//				transform->m[ 1 ][ 3 ],
//				transform->m[ 2 ][ 3 ]
				0,//transform( 0, 3 ),
				0,//transform( 1, 3 ),
				0//transform( 2, 3 )
				 );

			XMFLOAT3 relativePosition = XMFLOAT3(
				objPos.x - XMVectorGetX( this->_camera.CamPosition ),
				objPos.y - XMVectorGetY( this->_camera.CamPosition ),
				objPos.z - XMVectorGetZ( this->_camera.CamPosition )
			);
			
			XMFLOAT3 cameraForward = XMFLOAT3(
				XMVectorGetX( this->_camera.camForward ),
				XMVectorGetY( this->_camera.camForward ),
				XMVectorGetZ( this->_camera.camForward )
			);

			INT32 distanceFromCamera = ( INT32 )Dot( cameraForward, relativePosition );

			if ( distanceFromCamera < distanceToTest )
			{
				distanceToTest = distanceFromCamera;
			}
		}
	}

	INT32 lodIndex = 0;

	if ( distanceToTest >= this->lodSegments[ 0 ]
		 && distanceToTest < this->lodSegments[ 1 ] )
	{
		lodIndex = 1;
	}
	else if ( distanceToTest >= this->lodSegments[ 1 ]
				 && distanceToTest < this->lodSegments[ 2 ] )
	{
		lodIndex = 2;
	}
	else if ( distanceToTest >= this->lodSegments[ 2 ] )
	{
		lodIndex = 3;
	}

	*/
	INT32 lodIndex = 0;
	switch ( lodIndex )
	{
		case 0:
		{
			this->_meshDx->Draw(
				this->_d3d11DevCon, 
				viewProjection,
				this->_materialsList, 
				this->_shaderManager,
				this->rotation,
				this->position,
				this->scale,
//				listM,//this->_transforms->at( index ),
//				amount, 
				this->_light );
		}
		break;

		case 1:
		{
			_lstEditorObject3Ds[ LodId ]->_meshDx->DrawInstanced(
				this->_d3d11DevCon,
				viewProjection,
				this->_materialsList,
				this->_shaderManager,
				listM,
				amount,
				this->_light );
		}
		break;

		case 2:
		{
			this->_bBox->_meshDx->DrawInstanced(
				this->_d3d11DevCon,
				viewProjection,
				this->_materialsList,
				this->_shaderManager,
				listM,
				amount,
				this->_light );
		}
		break;

		default:
			break;
	}
}