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
	XMCOLOR color
)
{
	// defaults
	this->FilePath = "";
	this->IsLod = false;
	this->LodId = -1;
	this->InstanceId = -1;
	this->ProjectedDistanceFromCamera = 0;

	// set
	this->position = pos;
	this->rotation = rot;
	this->scale = scale;

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


	XMFLOAT3 minPoint;
	XMFLOAT3 maxPoint;

	if ( inspireUtils.ObjRead(
		d3d11Device, 
		shaderManager, 
		fileName, 
		pathModels, 
		materialsList, 
		&this->_mesh,
		&minPoint,
		&maxPoint 
		) )
	{
		// Create Bounding Box
		
		if ( useBboxLod )
		{
			this->_bBox = new EditorMesh( 
				d3d11DevCon,
				d3d11Device,
				"BBox.obj",
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
				color );
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
		MessageBox( 0, L"Scene Initialization - Failed",
			L"Error", MB_OK );
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
	XMCOLOR color
)
{
	// defaults
	this->FilePath = "";
	this->IsLod = false;
	this->LodId = -1;
	this->InstanceId = -1;
	this->ProjectedDistanceFromCamera = 0;

	// set
	this->position = pos;
	this->rotation = rot;
	this->scale = scale;

	this->_mesh = Mesh( );
	
	inspireUtils.ReadMaterialIdName( 
		d3d11Device, 
		&this->_mesh, pathModels, 
		"usemtl " + albedoName, 
		materialsList.size( ), 
		materialsList, 
		shaderManager,
		0 );

	for ( size_t i = 0; i < indices.size( ); i += 3 )
	{
		DXFace face = DXFace(
			indices[ i ], indices[ i + 1 ], indices[ i + 2 ],
			indices[ i ], indices[ i + 1 ], indices[ i + 2 ],
			indices[ i ], indices[ i + 1 ], indices[ i + 2 ], inspireUtils.CurrentId );

		this->_mesh.Faces.push_back( face );
	}

	for ( size_t i = 0; i < v.size( ); i++ )
	{
		XMFLOAT3 vertex = XMFLOAT3( v.at( i ).pos );
		this->_mesh.vertices.push_back( vertex );

		XMFLOAT2 uv = XMFLOAT2( v.at( i ).texCoord );
		this->_mesh.uvs.push_back( uv );

		XMFLOAT3 normal = XMFLOAT3( v.at( i ).normal );
		this->_mesh.normals.push_back( normal );
	}

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
	/*
	XMFLOAT3 minPoint;
	XMFLOAT3 maxPoint;

	if ( useBboxLod )
	{
		this->_bBox = new EditorMesh( d3d11DevCon,
									d3d11Device,
									"BBox.obj",
									pathModels,
									rot,
									pos,
									scale,
									false,
									materialsList,
									lstEditorObject3Ds,
									camera,
									shaderManager,
									light,
									inspireUtils,
									color );
	}
	*/
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
	XMFLOAT3 rot,
	XMFLOAT3 pos,
	XMFLOAT3 scale,
	Light &light,
	INT32 lodIndex )
{
	switch ( lodIndex )
	{
		case 0:
		{
			if ( this->InstanceId != -1 )
			{
				lstEditorObject3Ds.at( this->InstanceId )->_bBox->position = pos;
				lstEditorObject3Ds.at( this->InstanceId )->_bBox->rotation = rot;

				lstEditorObject3Ds.at( this->InstanceId )->_meshDx->Draw( d3d11DevCon,
																	viewProjection,
																	materialsList,
																	shaderManager,
																	rot,
																	pos,
																	scale,
																	light );
			}
			else
			{
				this->_meshDx->Draw( d3d11DevCon,
									viewProjection,
									materialsList,
									shaderManager,
									rot,
									pos,
									scale,
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
					rot,
					pos,
					scale,
					light );
			}
			break;
		}

		case 2:
		{
			if ( InstanceId != -1 )
			{
				lstEditorObject3Ds.at( this->InstanceId )->_bBox->position = pos;
				lstEditorObject3Ds.at( this->InstanceId )->_bBox->rotation = rot;

				lstEditorObject3Ds.at( this->InstanceId )->_bBox->_meshDx->Draw(
					d3d11DevCon,
					viewProjection,
					materialsList,
					shaderManager,
					rot,
					pos,
					scale,
					light );
			}
			else
			{
				this->_bBox->_meshDx->Draw(
									d3d11DevCon,
									viewProjection,
									materialsList,
									shaderManager,
									rot,
									pos,
									scale,
									light );
			}
			break;
		}

		default:
		{
		}
		break;
	}
}

/// <summary>
/// Render the 3D object
/// Select the correct LOD to render
/// </summary>
/// <param name="d3d11DevCon">DirectX Device context</param>
/// <param name="viewProjection">The Camera view projetion</param>
/// <param name="materialsList">The World Material List</param>
/// <param name="lstEditorObject3Ds">The list of Objects of the World</param>
/// <param name="shaderManager">The World Shader Manager</param>
/// <param name="blendManager">The World Blend Manager</param>
/// <param name="rot">rotation</param>
/// <param name="pos">position</param>
/// <param name="scale">scale</param>
/// <param name="lodIndex">lod Index</param>
void EditorMesh::RenderObject(
	ID3D11DeviceContext &d3d11DevCon,
	XMMATRIX viewProjection,
	std::vector<SurfaceMaterial> &materialsList,
	std::vector<EditorMeshPtr> &lstEditorObject3Ds,
	DXShaderManager &shaderManager,
	XMFLOAT3 rot,
	XMFLOAT3 pos,
	XMFLOAT3 scale,
	Light &light )
{
	if ( !IsLod )
	{
		//if ( this->_bBox->useBboxLod )
		{
			INT32 lodIndex = 0;// LodId != -1 ? LodManager.LodSegment( ProjectedDistanceFromCamera ) : 0;

			Render(
				d3d11DevCon,
				viewProjection,
				materialsList,
				lstEditorObject3Ds,
				shaderManager,
				rot,
				pos,
				scale, 
				light, 
				lodIndex );
		}
	}
}



EditorMesh::~EditorMesh( )
{
}

/*
 

        #region Methods

 

        /// <summary>

        /// Assign all vertex a color

        /// </summary>

        /// <param name="color">The color to Assign</param>

        public void Colorize(Color color)

        {

            _object3D.Colors = new List<Color>(_object3D.Points.Count);

            for (Int32 i = 0; i < _object3D.Points.Count; i++)

            {

                _object3D.Colors.Add(color);

            }

        }

 

 

        /// <summary>

        /// Scale the Model

        /// </summary>

        /// <param name="scale">the rescale parameter</param>

        public void Rescale(Double scale)

        {

            for (var i = 0; i < _object3D.Points.Count; i++)

            {

                _object3D.Points[i] /= scale;

            }

        }

 

        /// <summary>

        /// Set Position and Rotation

        /// </summary>

        /// <param name="pos"></param>

        /// <param name="rot"></param>

        /// <param name="scale"></param>

        /// <param name="useBBoxLod"></param>

        public void SetPositionRotation(Point pos, Point rot, Point scale, Boolean useBBoxLod)

        {

            Pos[0] = pos[0];

            Pos[1] = pos[1];

            Pos[2] = pos[2];

 

            Scale[0] = scale[0];

            Scale[1] = scale[1];

            Scale[2] = scale[2];

 

            Rot[0] = rot[0];

            Rot[1] = rot[1];

            Rot[2] = rot[2];

 

            Bbox.UseBBoxAsLod = useBBoxLod;

            Bbox.Pos = Pos;

            Bbox.Rot = Rot;

        }

 

        #endregion

    }

}

*/