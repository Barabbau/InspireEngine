#include "SceneGraph.h"

SceneGraph::SceneGraph(
	ID3D11DeviceContext &d3d11DevCon,
	ID3D11Device &d3d11Device,
	std::string fileName,
	std::vector<SurfaceMaterial> &materialsList,
	std::vector<EditorMeshPtr> &lstEditorObject3Ds,
	Camera &camera,
	DXShaderManager &shaderManager,
	Light &light,
	InspireUtils &inspireUtils )
{
	this->InstancedObjects = new std::unordered_map<std::string, EditorMeshInstancedPtr>( );

	SceneRead( d3d11DevCon,
							d3d11Device,
							fileName,
							materialsList,
							lstEditorObject3Ds,
							camera,
							shaderManager,
							light,
							inspireUtils );
}

SceneGraph::~SceneGraph( )
{
}

/// <summary>
/// Read Scene file
/// </summary>
/// <param name="fileName">the input file to read</param>
/// <param name="materials">The list of material names</param>
/// <returns>True if the model is correctly loaded</returns>
bool SceneGraph::SceneRead(
	ID3D11DeviceContext &d3d11DevCon,
	ID3D11Device &d3d11Device,
	std::string fileName,
	std::vector<SurfaceMaterial> &materialsList,
	std::vector<EditorMeshPtr> &lstEditorObject3Ds,
	Camera &camera,
	DXShaderManager &shaderManager,
	Light &light,
	InspireUtils &inspireUtils )
{
	// Get non-case sensitive name
	std::string objName = fileName;

	for ( char &c : objName )
	{
		c = inspireUtils.to_lowercase( c );
	}

	// Open the text file
	try
	{
		INT32 materialId = 0;
		std::vector<std::string> lines;

		std::ifstream in( objName, std::ios::in );

		if ( !in )
		{
			std::cerr << "Cannot open " << objName << std::endl;
			exit( 1 );
		}

		std::string lineTemp;
		while ( std::getline( in, lineTemp ) )
		{
			lines.push_back( lineTemp );
		}

		for ( size_t i = 0; i < lines.size( ); i++ )
		{
			std::vector<std::string> parts = inspireUtils.split( lines.at( i ), ';' );
			int partIndex = 0;

			if ( parts.at( partIndex )._Equal( "" ) )
			{
				partIndex = 1;
			}

			// objectName
			std::string currentObjName = parts.at( partIndex ) + ".obj";
			partIndex++;

			float positionScaler = 304.8f;
			float degreesToRadiansConverter = 0.01745329f;

			// position
			XMFLOAT3 pos;
			std::string posString = parts.at( partIndex );
			std::vector<std::string> posParts = inspireUtils.split( posString, ' ' );
			pos.x = std::atof( posParts.at( 0 ).substr( 2, posParts.at( 0 ).size( ) - 2 ).c_str( ) ) / positionScaler;
			pos.z = std::atof( posParts.at( 1 ).substr( 2, posParts.at( 1 ).size( ) - 2 ).c_str( ) ) / positionScaler;
			pos.y = std::atof( posParts.at( 2 ).substr( 2, posParts.at( 2 ).size( ) - 2 ).c_str( ) ) / positionScaler;
			partIndex++;

			XMFLOAT3 rot;
			std::string rotString = parts.at( partIndex );
			std::vector<std::string> rotParts = inspireUtils.split( rotString, ' ' );
			rot.x = std::atof( rotParts.at( 0 ).substr( 2, rotParts.at( 0 ).size( ) - 2 ).c_str( ) ) * degreesToRadiansConverter;
			rot.y = std::atof( rotParts.at( 1 ).substr( 2, rotParts.at( 1 ).size( ) - 2 ).c_str( ) ) * degreesToRadiansConverter;
			rot.z = std::atof( rotParts.at( 2 ).substr( 2, rotParts.at( 2 ).size( ) - 2 ).c_str( ) ) * degreesToRadiansConverter;
			partIndex++;

			XMFLOAT3 scale;
			std::string scaleString = parts.at( partIndex );
			std::vector<std::string> scaleParts = inspireUtils.split( scaleString, ' ' );
			scale.x = std::atof( scaleParts.at( 0 ).substr( 2, scaleParts.at( 0 ).size( ) - 2 ).c_str( ) );
			scale.y = std::atof( scaleParts.at( 1 ).substr( 2, scaleParts.at( 1 ).size( ) - 2 ).c_str( ) );
			scale.z = std::atof( scaleParts.at( 2 ).substr( 2, scaleParts.at( 2 ).size( ) - 2 ).c_str( ) );
			partIndex++;
			//camera.CamPosition = XMLoadFloat3( &pos );
			auto got = this->InstancedObjects->find( currentObjName );
			if ( got == this->InstancedObjects->end( ) )
			{
				EditorMeshInstanced *editorMeshInstanced = new EditorMeshInstanced(
					d3d11DevCon,
					d3d11Device,
					currentObjName,// "BBox.obj",//"Building_15stores_HouseOffice.obj",
					".\\Resources",
					rot,
					pos,
					scale,
					true,
					materialsList,
					lstEditorObject3Ds,
					camera,
					shaderManager,
					light,
					inspireUtils );

				if ( editorMeshInstanced->_mesh.vertices.size( ) > 0 )
				{
					EditorMeshInstancedPtr newComPointer( editorMeshInstanced );
					// add new object pair
					std::pair<std::string, EditorMeshInstancedPtr> newEditorMeshInstancedPair( currentObjName, newComPointer );
					this->InstancedObjects->insert( newEditorMeshInstancedPair );
				}
			}
			else
			{
				// add a transform to the existing instanciable object
				this->InstancedObjects->at( currentObjName )->AddSpawnPoint( pos, rot, scale );
			}
		}

		for ( auto it : *this->InstancedObjects )
		{
			it.second->CreateTransforms( );
		}
		return true;
	}
	catch ( int e )
	{
		sprintf( "Unable to open Model: %s\n", fileName.c_str( ) );

		return false;
	}
}

void SceneGraph::Render( XMMATRIX ViewPerspective )
{
	for ( auto it : *this->InstancedObjects )
	{
		it.second->RenderInstanced( ViewPerspective );
	}
}