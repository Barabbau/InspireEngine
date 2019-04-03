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
		/*
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

EditorMesh::~EditorMesh( )
{
}

/*
        /// <summary>

        /// Loads an Object 3D using the serializer

        /// </summary>

        /// <param name="log">The ILog interface</param>

        /// <param name="device">DirectX Device</param>

        /// <param name="fileName">The file name to load in the Object 3D</param>

        /// <param name="pathModels">std App path</param>

        /// <param name="pos">The position for the Object</param>

        /// <param name="rot">The rotation of the Object</param>

        /// <param name="scale">The object scale</param>

        /// <param name="useBboxLod">Should use the BBox for farthest LOD</param>

        /// <param name="materials">The list of Material names of the World</param>

        /// <param name="lstObjects">The list of Objects of the World</param>

        /// <param name="color"></param>

        /// <param name="materialType"></param>

        /// <returns></returns>

        public EditorObject3D(ILog log,

                              Device device,

                              String fileName,

                              String pathModels,

                              Point pos,

                              Point rot,

                              Point scale,

                              Boolean useBboxLod,

                              ref Dictionary<String, MaterialManager.MaterialType> materials,

                              ref List<EditorObject3D> lstObjects,

                              Color color = default,

                              MaterialManager.MaterialType materialType = MaterialManager.MaterialType.Standard)

        {

            if (color == default)

            {

                color = Color.White;

            }

 

            FilePath = "";

 

            IsLod = false;

            LodId = -1;

            InstanceId = -1;

 

            Pos = pos;

            Rot = rot;

            Scale = scale;

 

            ProjectedDistanceFromCamera = 0;

 

            _object3D = new Object3D();

 

            _drawable3D = new Drawable3D(_object3D);

 

            Bbox = new BBox();

 

            String sFileName = LodManager.LOD_PREFIX + fileName;

            String sLodPath = Path.Combine(pathModels, sFileName);

 

            // Load the LOD if available

            if (File.Exists(sLodPath))

            {

                log.Debug("Found LOD for Model: " + fileName);

 

                var aObject = new EditorObject3D(log, device, sFileName, pathModels, pos, rot, scale, false, ref materials, ref lstObjects, default, materialType)

                              {

                                  Bbox = {UseBBoxAsLod = false},

                                  IsLod = true

                              };

 

                // if instance of an existing LOD than use the same Object3D for the LOD

                if (aObject.InstanceId != -1)

                {

                    LodId = aObject.InstanceId;

                }

                // else create a new ID for the new LOD Object3D

                else

                {

                    lstObjects.Add(aObject);

                    LodId = lstObjects.Count - 1;

                }

            }

 

            // Get non-case sensitive name

            String objName = Path.Combine(pathModels, fileName);

            objName = objName.ToLower();

            FilePath = objName;

 

            // check if it has already been loaded

            var instanceId = 0;

 

            foreach (EditorObject3D aObject in lstObjects)

            {

                if (!String.IsNullOrEmpty(aObject.FilePath))

                {

                    if (aObject.FilePath == objName)

                    {

                        InstanceId = instanceId;

 

                        if (_object3D.Normals.Count != _object3D.Points.Count)

                        {

                            MeshUtilities.ComputeNormals(_object3D);

                        }

 

                        Colorize(color);

 

                        Drawable3D.CreateVertexBufferArray(device);

 

                        SetPositionRotation(pos, rot, scale, useBboxLod);

                        return;

                    }

                }

 

                instanceId++;

            }

 

            if (GeometrySerializer.Read(log, fileName, pathModels, ref materials, ref _object3D, out Point minVertex, out Point maxVertex, materialType))

            {

                // Create Bounding Box

                if (useBboxLod)

                {

                    String bboxPath = Path.Combine(TerrainEditorUtilities.AppPath, TerrainEditorUtilities.PATH_MODEL);

                    Bbox = new BBox(device, minVertex, maxVertex, pos, bboxPath, ref materials);

                }

 

                if (_object3D.Normals.Count != _object3D.Points.Count)

                {

                    MeshUtilities.ComputeNormals(_object3D);

                }

 

                Colorize(color);

 

                Drawable3D.CreateVertexBufferArray(device);

 

                SetPositionRotation(pos, rot, scale, useBboxLod);

            }

        }

 

        public EditorObject3D()

        {

            _object3D = new Object3D();

            InstanceId = -1;

            FilePath = "";

            LodId = -1;

            IsLod = false;

            Pos = Point.Zero();

            Rot = Point.Zero();

            Scale = Point.One();

 

            ProjectedDistanceFromCamera = 0;

 

            Bbox = new BBox();

 

            _drawable3D = new Drawable3D(_object3D);

        }

 

        #endregion

 

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

        public void Render(Device device,

                           BaseCamera baseCamera,

                           Dictionary<String, MaterialDx> materialDictionary,

                           ShaderManager shaderManager,

                           ref List<EditorObject3D> lstObjects,

                           Int32 lodIndex,

                           BlendManager blendManager)

        {

            switch (lodIndex)

            {

                case 0:

                {

                    if (InstanceId != -1)

                    {

                        lstObjects[InstanceId].Bbox.Pos = Pos;

                        lstObjects[InstanceId].Bbox.Rot = Rot;

                        lstObjects[InstanceId].Drawable3D.Draw(device, baseCamera, materialDictionary, shaderManager, blendManager, Rot, Pos, Scale);

                    }

                    else

                    {

                        _drawable3D.Draw(device, baseCamera, materialDictionary, shaderManager, blendManager, Rot, Pos, Scale);

                    }

 

                    break;

                }

 

                case 1:

                {

                    if (LodId != -1)

                    {

                        lstObjects[LodId].Drawable3D.Draw(device, baseCamera, materialDictionary, shaderManager, blendManager, Rot, Pos, Scale);

                    }

 

                    break;

                }

 

 

                case 2:

                {

                    if (InstanceId != -1)

                    {

                        lstObjects[InstanceId].Bbox.Pos = Pos;

                        lstObjects[InstanceId].Bbox.Rot = Rot;

                        lstObjects[InstanceId].Bbox.Draw(device, baseCamera, materialDictionary, shaderManager);

                    }

                    else

                    {

                        Bbox.Draw(device, baseCamera, materialDictionary, shaderManager);

                    }

 

                    break;

                }

 

                default: { }

                    break;

            }

        }

 

        /// <summary>

        /// Render the 3D object

        /// Select the correct LOD to render

        /// </summary>

        /// <param name="device">DirectX Device</param>

        /// <param name="baseCamera">The world Camera</param>

        /// <param name="materialDictionary">The World Material Dictionary</param>

        /// <param name="shaderManager">The World Shader Manager</param>

        /// <param name="blendManager">The World Blend Manager</param>

        /// <param name="lstObjects">The list of Objects of the World</param>

        public void RenderObject(Device device, BaseCamera baseCamera, Dictionary<String, MaterialDx> materialDictionary, ShaderManager shaderManager, BlendManager blendManager, ref List<EditorObject3D> lstObjects)

        {

            if (!IsLod)

            {

                if (Bbox.UseBBoxAsLod)

                {

                    Int32 lodIndex = LodId != -1 ? LodManager.LodSegment(ProjectedDistanceFromCamera) : 0;

 

                    Render(device, baseCamera, materialDictionary, shaderManager, ref lstObjects, lodIndex, blendManager);

                }

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