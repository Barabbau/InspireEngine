#pragma once

#include "EditorMesh.h"

EditorMesh::EditorMesh(){ }

EditorMesh::EditorMesh(
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
	DXShaderManager &shaderManager,
	Light &light,
	InspireUtils &inspireUtils,
	INT32 materialType,
	EditorMeshPtr bBox,
	XMCOLOR color
)
{
	// defaults
	this->FilePath = "";
	this->IsLod = false;
	this->LodId = -1;
	this->InstanceId = -1;
	this->ProjectedDistanceFromCamera = 0;
	this->BBox = bBox;


	//Define world space matrix
	XMMATRIX Rotation = XMMatrixRotationRollPitchYaw( rot.x, rot.y, rot.z );
	XMMATRIX Scale = XMMatrixScaling( scale.x, scale.y, scale.z );
	XMMATRIX Translation = XMMatrixTranslation( pos.x, pos.y, -pos.z );

	this->Transform = Scale * Rotation * Translation;

	this->_mesh = Mesh( );

	std::string sFileName = "LOD01_" + fileName;
	std::string sLodPath = pathModels + "\\" + sFileName;

	////////////////////////////
	// Load the LOD if available
	std::ifstream in( sLodPath, std::ios::in );

	if ( !in )
	{
		std::cerr << "NO LOD for Model: " << sFileName << std::endl;
	}
	else
	{
		std::cerr << ( "Found LOD for Model: " + fileName ).c_str( ) << std::endl;

		EditorMesh* aObject = new EditorMesh( d3d11DevCon,
											 d3d11Device,
											 sFileName,
											 pathModels,
											 rot,
											 pos,
											 scale,
											 false,
											 materialsList,
											 lstEditorObject3Ds,
											 shaderManager,
											 light,
											 inspireUtils,
											  materialType,
											  bBox,
										 color );
		aObject->IsLod = true;

		// if instance of an existing LOD than use the same Object3D for the LOD
		if ( aObject->InstanceId != -1 )
		{
			LodId = aObject->InstanceId;
		}
		// else create a new ID for the new LOD Object3D
		else
		{
			EditorMeshPtr aSmartObject( aObject );
			lstEditorObject3Ds.push_back( aSmartObject );

			LodId = lstEditorObject3Ds.size( ) - 1;
		}
	}
	/////////////////////////



	// Get non-case sensitive name
	std::string objName = pathModels + "\\" + fileName;

	for ( char &c : objName )
	{
		c = inspireUtils.to_lowercase( c );
	}

	this->FilePath = objName;

	// check if it has already been loaded
	INT32 instanceId = 0;

	for ( size_t i = 0; i < lstEditorObject3Ds.size( ); i++ )
	{
		if ( !lstEditorObject3Ds[ i ]->FilePath._Equal("") )
		{
			if ( lstEditorObject3Ds[ i ]->FilePath._Equal( objName ) )
			{
				this->InstanceId = instanceId;
				this->minPoint = lstEditorObject3Ds.at( this->InstanceId )->minPoint;
				this->maxPoint = lstEditorObject3Ds.at( this->InstanceId )->maxPoint;

				return;
			}
		}

		instanceId++;
	}

	XMFLOAT3 min;
	XMFLOAT3 max;

	if ( inspireUtils.ObjRead(
		d3d11Device, 
		shaderManager, 
		fileName, 
		pathModels, 
		materialsList, 
		&this->_mesh,
		min,
		max,
		materialType
		) )
	{


		if ( this->InstanceId == -1 )
		{
			EditorMeshPtr aSmartObject( this );
			lstEditorObject3Ds.push_back( aSmartObject );

			this->minPoint = XMLoadFloat3( &min );
			this->maxPoint = XMLoadFloat3( &max );
		}
		else
		{
			this->minPoint = lstEditorObject3Ds.at( this->InstanceId )->minPoint;
			this->maxPoint = lstEditorObject3Ds.at( this->InstanceId )->maxPoint;
		}



		/*
		if ( this->_mesh.normals.size( ) != this->_mesh.vertices.size( ) )
		{
			this->_mesh.ComputeNormals( );
		}
		*/
		this->_mesh.ComputeTangents( );
		
		this->_meshDx = new MeshDX( _mesh );
		this->_meshDx->CreateVertexBufferArray( d3d11Device );

		this->useBboxLod = useBboxLod;
	}
	else
	{
//		MessageBox( 0, L"Scene Initialization - Failed", L"Error", MB_OK );
		return;
	}
}

EditorMesh::EditorMesh(
	ID3D11DeviceContext &d3d11DevCon,
	ID3D11Device &d3d11Device,
	std::string objectName,
	std::string pathModels,
	std::string albedoName,
	std::vector<DXVertex> &v,
	std::vector<DWORD> &indices,
	XMFLOAT3 rot,
	XMFLOAT3 pos,
	XMFLOAT3 scale,
	bool useBboxLod,
	std::vector<SurfaceMaterial> &materialsList,
	std::vector<EditorMeshPtr> &lstEditorObject3Ds,
	DXShaderManager &shaderManager,
	Light &light,
	InspireUtils &inspireUtils,
	INT32 materialType,
	EditorMeshPtr bBox,
	XMCOLOR color
)
{
	// defaults
	this->FilePath = "";
	this->IsLod = false;
	this->LodId = -1;
	this->InstanceId = -1;
	this->ProjectedDistanceFromCamera = 0;

	//Define world space matrix
	XMMATRIX Rotation = XMMatrixRotationRollPitchYaw( rot.x, rot.y, rot.z );
	XMMATRIX Scale = XMMatrixScaling( scale.x, scale.y / 100, scale.z );
	XMMATRIX Translation = XMMatrixTranslation( pos.x, pos.y, -pos.z );

	this->Transform = Scale * Rotation * Translation;

	this->_mesh = Mesh( );
	
	inspireUtils.ReadMaterialIdName( 
		d3d11Device, 
		&this->_mesh, pathModels, 
		"usemtl " + albedoName, 
		materialsList.size( ), 
		materialsList, 
		shaderManager,
		materialType );

	for ( size_t i = 0; i < indices.size( ); i += 3 )
	{
		DXFace face = DXFace(
			indices[ i ], indices[ i + 1 ], indices[ i + 2 ],
			indices[ i ], indices[ i + 1 ], indices[ i + 2 ],
			indices[ i ], indices[ i + 1 ], indices[ i + 2 ], inspireUtils.CurrentId );

		this->_mesh.Faces.push_back( face );
	}

	XMFLOAT3 minVertex = XMFLOAT3( 100000, 100000, 100000 );
	XMFLOAT3 maxVertex = XMFLOAT3( -100000, -100000, -100000 );


	for ( size_t i = 0; i < v.size( ); i++ )
	{
		XMFLOAT3 vertex = XMFLOAT3( v.at( i ).pos );
		this->_mesh.vertices.push_back( vertex );

		XMFLOAT2 uv = XMFLOAT2( v.at( i ).texCoord );
		this->_mesh.uvs.push_back( uv );

		XMFLOAT3 normal = XMFLOAT3( v.at( i ).normal );
		this->_mesh.normals.push_back( normal );

		// Calculate mix and max for the bounding box
		if ( vertex.x < minVertex.x )
		{
			minVertex.x = vertex.x;
		}

		if ( vertex.y < minVertex.y )
		{
			minVertex.y = vertex.y;
		}

		if ( vertex.z < minVertex.z )
		{
			minVertex.z = vertex.z;
		}


		if ( vertex.x > maxVertex.x )
		{
			maxVertex.x = vertex.x;
		}

		if ( vertex.y > maxVertex.y )
		{
			maxVertex.y = vertex.y;
		}

		if ( vertex.z > maxVertex.z )
		{
			maxVertex.z = vertex.z;
		}
	}

	this->minPoint = XMLoadFloat3( &minVertex );
	this->maxPoint = XMLoadFloat3( &maxVertex );

	// Get non-case sensitive name
	std::string objName = objectName;

	for ( char &c : objName )
	{
		c = inspireUtils.to_lowercase( c );
	}

	this->FilePath = objName;

	// check if it has already been loaded
	INT32 instanceId = 0;

	for ( size_t i = 0; i < lstEditorObject3Ds.size( ); i++ )
	{
		if ( !lstEditorObject3Ds[ i ]->FilePath._Equal( "" ) )
		{
			if ( lstEditorObject3Ds[ i ]->FilePath._Equal( objName ) )
			{
				InstanceId = instanceId;

				if ( this->_mesh.normals.size( ) != this->_mesh.vertices.size( ) )
				{
					this->_mesh.ComputeNormals( );
				}

				this->_meshDx = new MeshDX( _mesh );
				this->_meshDx->CreateVertexBufferArray( d3d11Device );

				this->useBboxLod = useBboxLod;

				return;
			}
		}

		instanceId++;
	}



	// Create Bounding Box
	this->BBox = bBox;
	
	/*
	if ( this->_mesh.normals.size( ) != this->_mesh.vertices.size( ) )
	{
		this->_mesh.ComputeNormals( );
	}
	*/
	this->_mesh.ComputeTangents( );


	this->_meshDx = new MeshDX( this->_mesh );
	this->_meshDx->CreateVertexBufferArray( d3d11Device );

	this->useBboxLod = useBboxLod;

}


/// <summary>
/// Render the Object
/// Select the correct LOD or the Bounding Box to render
/// </summary>
/// <param name="device">DirectX Device</param>
/// <param name="baseCamera">The world Camera</param>
/// <param name="materialDictionary">The World Material Dictionary</param>
/// <param name="shaderManager">The World Shader Manager</param>
/// <param name="lstObjects">The list of Objects of the World</param>
/// <param name="lodIndex">The LOD index to be rendered</param>
/// <param name="blendManager">The World Blend Manager</param>
void EditorMesh::Render(
	ID3D11DeviceContext &d3d11DevCon,
	XMMATRIX viewProjection,
	std::vector<SurfaceMaterial> &materialsList,
	std::vector<EditorMeshPtr> &lstEditorObject3Ds,
	DXShaderManager &shaderManager,
	Light &light )
{
	switch ( this->LodIndex )
	{
		case 0:
		{
			if ( this->InstanceId != -1 )
			{
				lstEditorObject3Ds.at( this->InstanceId )->_meshDx->Draw( d3d11DevCon,
																	viewProjection,
																	materialsList,
																	shaderManager,
																	this->Transform,
																	light );
			}
			else
			{
				this->_meshDx->Draw( d3d11DevCon,
									viewProjection,
									materialsList,
									shaderManager,
									this->Transform,
									light );
			}
			break;
		}

		case 1:
		{
			if ( LodId != -1 )
			{
				lstEditorObject3Ds.at( this->LodId )->_meshDx->Draw( 
					d3d11DevCon,
					viewProjection,
					materialsList,
					shaderManager,
					this->Transform,
					light );
			}
			break;
		}

		case 2:
		{
			this->BBox->_meshDx->Draw(
								d3d11DevCon,
								viewProjection,
								materialsList,
								shaderManager,
								this->Transform,
								light );
			break;
		}

		default:
		{
		}
		break;
	}
}

void EditorMesh::RenderDepth(
	ID3D11DeviceContext &d3d11DevCon,
	XMMATRIX viewProjection,
	DXShader &shader,
	std::vector<SurfaceMaterial> &materialsList,
	std::vector<EditorMeshPtr> &lstEditorObject3Ds,
	DXShaderManager &shaderManager )
{
	switch ( this->LodIndex )
	{
		case 0:
		{
			if ( this->InstanceId != -1 )
			{
				lstEditorObject3Ds.at( this->InstanceId )->_meshDx->DrawDepth( 
					d3d11DevCon,
					shader,
					materialsList,
					viewProjection,
					shaderManager,
					this->Transform );
			}
			else
			{
				this->_meshDx->DrawDepth( 
					d3d11DevCon,
					shader,
					materialsList,
					viewProjection,					
					shaderManager,
					this->Transform );
			}
			break;
		}

		case 1:
		{
			if ( LodId != -1 )
			{
				lstEditorObject3Ds.at( this->LodId )->_meshDx->DrawDepth(
					d3d11DevCon,
					shader,
					materialsList,
					viewProjection,
					shaderManager,
					this->Transform );
			}
			break;
		}

		case 2:
		{
			this->BBox->_meshDx->DrawDepth(
				d3d11DevCon,
				shader,
				materialsList,
				viewProjection,
				shaderManager,
				this->Transform );
			break;
		}

		default:
		{
		}
		break;
	}
}

/// <summary>
/// CalculateLod
/// Select the correct LOD to render
/// </summary>
void EditorMesh::CalculateLod(
	XMVECTOR cameraPosition,
	XMVECTOR cameraForward )
{
	if ( !this->IsLod )
	{	
		XMVECTOR objPosition = XMVectorSet(
				this->Transform._41,
				this->Transform._42,
				this->Transform._43,
				0 );

		XMVECTOR vectorDistance = XMVectorSubtract( cameraPosition, objPosition );

		XMVECTOR length = XMVector3Length( vectorDistance );

		float distance = 0.0f;
		XMStoreFloat( &distance, length );

		INT32 distanceFromCamera = ( INT32 ) distance;


		XMVECTOR min = XMVector3Transform( this->minPoint, this->Transform );
		XMVECTOR max = XMVector3Transform( this->maxPoint, this->Transform );
		XMVECTOR vectorSize = XMVectorSubtract( min, max );

		XMVECTOR size = XMVector3Length( vectorSize );

		float maxSize = 0.0f;
		XMStoreFloat( &maxSize, size );

		bool isCameraInsideSphere = distanceFromCamera < ( maxSize / 2.0f );

		float dot = 1;

		// CAMERA CULLING
		// Use all the point of the non AABB bbox for culling if closer then 200 units and the camera is inside the object sphere
		if ( distanceFromCamera < 200 || isCameraInsideSphere )
		{

			XMFLOAT3 minMin;
			XMStoreFloat3( &minMin, min );

			XMFLOAT3 maxMax;
			XMStoreFloat3( &maxMax, max );

			int count = 0;
			
			// Test every point of the bounding box
			float dotToTest = isCameraInsideSphere ? 0.0f : -0.5f;
			while ( dot > dotToTest )
			{
				XMVECTOR vectorPos;
				XMFLOAT3 p;
				switch ( count )
				{
					case 0:
						p.x = minMin.x;
						p.y = minMin.y;
						p.z = minMin.z;
						vectorPos = XMLoadFloat3( &p );
						break;

					case 1:
						p.x = maxMax.x;
						p.y = maxMax.y;
						p.z = maxMax.z;
						vectorPos = XMLoadFloat3( &p );
						break;

					case 2:
						p.x = minMin.x;
						p.y = maxMax.y;
						p.z = minMin.z;
						vectorPos = XMLoadFloat3( &p );
						break;

					case 3:
						p.x = minMin.x;
						p.y = minMin.y;
						p.z = maxMax.z;
						vectorPos = XMLoadFloat3( &p );
						break;

					case 4:
						p.x = minMin.x;
						p.y = maxMax.y;
						p.z = maxMax.z;
						vectorPos = XMLoadFloat3( &p );
						break;

					case 5:
						p.x = maxMax.x;
						p.y = minMin.y;
						p.z = maxMax.z;
						vectorPos = XMLoadFloat3( &p );
						break;

					case 6:
						p.x = maxMax.x;
						p.y = minMin.y;
						p.z = minMin.z;
						vectorPos = XMLoadFloat3( &p );
						break;

					case 7:
						p.x = maxMax.x;
						p.y = maxMax.y;
						p.z = minMin.z;
						vectorPos = XMLoadFloat3( &p );
						break;

					default:
						return;
						break;
				}

				XMVECTOR vectorDistanceDetailed = XMVectorSubtract( cameraPosition, vectorPos );
				vectorDistanceDetailed = XMVector3Normalize( vectorDistanceDetailed );
				XMVECTOR objDot = XMVector3Dot( vectorDistanceDetailed, cameraForward );

				XMStoreFloat( &dot, objDot );

				/*
				this->BBox->_meshDx->Draw(
					d3d11DevCon,
					viewProjection,
					materialsList,
					shaderManager,
					XMMatrixScaling( 0.1f, 0.1f, 0.1f ) * XMMatrixTranslation( p.x, p.y, p.z ),
					light );
				*/
				count++;
			}
		}
		// Use only object center for culling if farther then 200 units and the camera is outside the object sphere
		else
		{
			vectorDistance = XMVector3Normalize( vectorDistance );
			XMVECTOR objDot = XMVector3Dot( vectorDistance, cameraForward );
			XMStoreFloat( &dot, objDot );

			if ( dot > 0.0f )
			{
				this->LodIndex = 3;
				return;
			}
		}

		/*
		// FRUSTUM CULLING
		bool clippedMin = false;
		bool clippedMax = false;

		XMVECTOR min = XMVector3Transform( this->minPoint, this->Transform );
		XMVECTOR max = XMVector3Transform( this->maxPoint, this->Transform );

		XMFLOAT3 minMin;
		XMStoreFloat3( &minMin, min );

		XMFLOAT3 maxMax;
		XMStoreFloat3( &maxMax, max );

		float nearClip =  0.01f;
		float farClip = 1000.0f;
		float halfWidth = 1000;
		float halfHeight = 540;

		if ( minMin.z > nearClip )clippedMin = true;
		if ( minMin.z < farClip )clippedMin = true;

		if ( maxMax.z > nearClip )clippedMax = true;
		if ( maxMax.z < farClip )clippedMax = true;

		float zcompWmin = minMin.z * -halfWidth;
		float zcompWmax = maxMax.z * -halfWidth;

		clippedMin = !( ( ( minMin.x * nearClip + zcompWmin ) > 0 ) && ( ( minMin.x * -nearClip + zcompWmin ) > 0 ) &&
			( ( minMin.y * -nearClip + minMin.z * -halfHeight ) > 0 ) && ( ( minMin.y * nearClip + minMin.z * -halfHeight ) > 0 ) );
	
		clippedMax = !( ( ( maxMax.x * nearClip + zcompWmax ) > 0 ) && ( ( maxMax.x * -nearClip + zcompWmax ) > 0 ) &&
			( ( maxMax.y * -nearClip + maxMax.z * -halfHeight ) > 0 ) && ( ( maxMax.y * nearClip + maxMax.z * -halfHeight ) > 0 ) );

		if ( clippedMin && clippedMax )
		*/
		this->LodIndex = 0;

		// LOD switch
		if ( distanceFromCamera >= this->lodSegments[ 0 ]
				&& distanceFromCamera < this->lodSegments[ 1 ]
				&& this->LodId != -1 )
		{
			this->LodIndex = 1;
		}
		else if ( distanceFromCamera >= this->lodSegments[ 1 ]
				&& distanceFromCamera < this->lodSegments[ 2 ] )
		{
			this->LodIndex = 2;
		}
		else if ( distanceFromCamera >= this->lodSegments[ 2 ] )
		{
			this->LodIndex = 3;
		}
	}
	else
	{
	    this->LodIndex = 3;
	}
}

EditorMesh::~EditorMesh( )
{
}