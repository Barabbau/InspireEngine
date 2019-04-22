#include "InspireUtils.h"

InspireUtils::InspireUtils( )
{
}

InspireUtils::~InspireUtils( )
{
}

/// <summary>
/// Convert char to lower
/// </summary>
char InspireUtils::to_lowercase( char c )
{
	if ( c >= 'A' && c <= 'Z' )
	{
		return c + 32;
	}

	return c;
}

int InspireUtils::StringToWString( std::wstring &ws, const std::string &s )
{
	std::wstring wsTmp( s.begin( ), s.end( ) );
	ws = wsTmp;

	return 0;
}

std::vector<std::string> InspireUtils::split( const std::string &s, char delim )
{
	std::stringstream ss( s );
	std::string item;
	std::vector<std::string> elems;
	while ( std::getline( ss, item, delim ) )
	{
		elems.push_back( item );
		// elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
	}
	return elems;
}


/// <summary>
/// Verifies the existence of a file
/// </summary>
/// <param name="name">file path</param>
/// <returns>true if a file exists</returns>
inline bool exists( const std::string& name )
{
	std::ifstream f( name.c_str( ) );
	return f.good( );
}

/// <summary>
/// Reads & load the material for the OBJ file
/// Verifies the existence of the same material to avoid duplications
/// reassigns the existing material id to the faces in case of existing material
/// </summary>
/// <param name="object3D"></param>
/// <param name="line">The text line of the input file</param>
/// <param name="materialId">the material Id</param>
/// <param name="materials">The list of material names</param>
/// <returns>true if a material was in the materials list already</returns>
bool InspireUtils::ReadMaterialIdName(
	ID3D11Device &d3d11Device,
	Mesh* object3D,
	const std::string &pathModels,
	const std::string &line,
	INT32 materialId,
	std::vector<SurfaceMaterial> &materialsList,
	DXShaderManager &shaderManager,
	INT32 materialType )
{
	// Load Material names
	if ( line.substr( 0, 7 ) == "usemtl " )
	{
		std::string strLineName = line.substr( 7, line.size( ) - 7 );

		bool bNew = true;

		// search if the material was added already
		for ( size_t mn = 0; mn < materialsList.size( ); mn++ )
		{
			std::string matName = materialsList.at( mn ).matName;

			for ( char &c : matName )
			{
				c = to_lowercase( c );
			}

			for ( char &c : strLineName )
			{
				c = to_lowercase( c );
			}

			if ( matName._Equal( strLineName ) )
			{
				bNew = false;
				materialId = mn;
				mn = materialsList.size( );
			}
		}

		// add the material name if new
		if ( bNew )
		{
			materialId = materialsList.size( );

			SurfaceMaterial newMaterial = SurfaceMaterial( );

			std::string filePathStd = pathModels + "maps\\" + strLineName + ".dds";

			if ( !exists( filePathStd ) )
			{
				//std::cout << "File not found : " << filePathStd << '\n';
				std::string debugOutputString = "ALBEDO File not found : " + filePathStd + '\n';
				std::wstring debugOutput;
				StringToWString( debugOutput, debugOutputString );
				OutputDebugString( debugOutput.c_str( ) );
				filePathStd = pathModels + "maps\\missing_texture.dds";
			}
			std::wstring filePath;
			StringToWString( filePath, filePathStd );

			HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
				&d3d11Device,
				filePath.c_str( ),
				NULL,
				NULL,
				&newMaterial.albedoTexture,
				NULL );


			std::string filePathNormal = pathModels + "maps\\" + strLineName + "_Normal.dds";

			if ( !exists( filePathNormal ) )
			{
				//std::cout << "File not found : " << filePathNormal << '\n';
				std::string debugOutputString = "NORMAL File not found : " + filePathNormal + '\n';
				std::wstring debugOutput;
				StringToWString( debugOutput, debugOutputString );
				OutputDebugString( debugOutput.c_str() );
				filePathNormal = pathModels + "maps\\missing_Normal.dds";
			}
			std::wstring normalPath;
			StringToWString( normalPath, filePathNormal );

			hr = D3DX11CreateShaderResourceViewFromFile(
				&d3d11Device,
				normalPath.c_str( ),
				NULL,
				NULL,
				&newMaterial.normalTexture,
				NULL );


			newMaterial.difColor = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			newMaterial.hasTexture = true;
			const std::string::size_type size = strLineName.size( );
			
			newMaterial.matName = new char[ size + 1 ];  //we need extra char for NUL
			memcpy( newMaterial.matName, strLineName.c_str( ), size + 1 );

			switch ( materialType )
			{
				case 0:
					newMaterial.shader = shaderManager._stdShader;
					break;

				case 1:
					newMaterial.shader = shaderManager._stdShaderInstanced;
					break;

				default:
					newMaterial.shader = shaderManager._stdShader;
					break;
			}

			newMaterial.transparent = false;

			materialsList.push_back( newMaterial );
		}

		// add
		bool toAdd = true;
		for ( size_t i = 0; i < object3D->MaterialIds.size( ); i++ )
		{
			if ( object3D->MaterialIds[ i ] == materialId )
			{
				toAdd = false;
				break;
			}
		}

		if ( toAdd )
		{
			object3D->MaterialIds.push_back( materialId );
		}

		CurrentId = materialId;

		return true;
	}

	return false;
}

/// <summary>
/// Read OBJ format
/// </summary>
/// <param name="fileName">the input file to read</param>
/// <param name="pathModels">models path</param>
/// <param name="materials">The list of material names</param>
/// <param name="object3D"></param>
/// <param name="minVertex">The Bbox min pos</param>
/// <param name="maxVertex">The Bbox max pos</param>
/// <param name="log">The Log Interface</param>
/// <param name="materialType"></param>
/// <returns>True if the model is correctly loaded</returns>
bool InspireUtils::ObjRead(
	ID3D11Device &d3d11Device,
	DXShaderManager &shaderManager,
	std::string fileName,
	std::string pathModels,
	std::vector<SurfaceMaterial> &materialsList,
	Mesh* object3D,
	XMFLOAT3 &minVertex,
	XMFLOAT3 &maxVertex,
	INT32 materialType )
{
	minVertex = XMFLOAT3( 100000, 100000, 100000 );
	maxVertex = XMFLOAT3( -100000, -100000, -100000 );

	// Get non-case sensitive name
	std::string objName = pathModels.append( "\\" );
	objName = objName.append( fileName );

	for ( char &c : objName )
	{
		c = to_lowercase( c );
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
			std::string debugOutputString = "MODEL File not found : " + objName + '\n';
			std::wstring debugOutput;
			StringToWString( debugOutput, debugOutputString );
			OutputDebugString( debugOutput.c_str( ) );
			return false;
//			exit( 1 );
		}
		std::string lineTemp;
		while ( std::getline( in, lineTemp ) )
		{
			lines.push_back( lineTemp );
		}

		bool hasFoundMaterial = false;

		// read the first material to set the first material Id as a key for our sub mesh dictionaries
		for ( size_t i = 0; i < lines.size( ); i++ )
		{
			if ( !hasFoundMaterial )
			{
				hasFoundMaterial = ReadMaterialIdName(
					d3d11Device,
					object3D,
					pathModels,
					lines[ i ],
					materialId,
					materialsList,
					shaderManager,
					materialType
				);
			}
			else
			{
				break;
			}
		}

		materialId = CurrentId;

		for ( size_t i = 0; i < lines.size( ); i++ )
		{
			std::string  line = lines[ i ];

			if ( line.size( ) > 0 )
			{
				int loc = -1;
				while ( ( loc = line.find( "  ", loc + 1 ) ) != std::string::npos ) //Two spaces here
				{
					line.replace( loc, 2, " " ); //Single space in quotes
				}

				// Load Vertexes
				if ( line.substr( 0, 2 ) == "v " )
				{
					std::istringstream v( line.substr( 2 ) );

					XMFLOAT3 vert;
					double x, y, z;
					v >> x;
					v >> y;
					v >> z;

					vert = XMFLOAT3( x, y, z * -1.0f );
					object3D->vertices.push_back( vert );

					//Get the smallest vertex 
					minVertex.x = min( minVertex.x, vert.x );    // Find smallest x value in model
					minVertex.y = min( minVertex.y, vert.y );    // Find smallest y value in model
					minVertex.z = min( minVertex.z, vert.z );    // Find smallest z value in model

					//Get the largest vertex 
					maxVertex.x = max( maxVertex.x, vert.x );    // Find largest x value in model
					maxVertex.y = max( maxVertex.y, vert.y );    // Find largest y value in model
					maxVertex.z = max( maxVertex.z, vert.z );    // Find largest z value in model

					continue;
				}

				// Load UVs
				if ( line.substr( 0, 3 ) == "vt " )
				{
					std::istringstream v( line.substr( 2 ) );

					XMFLOAT2 uv;
					double x, y;
					v >> x;
					v >> y;

					uv = XMFLOAT2( x, 1.0f - y );
					object3D->uvs.push_back( uv );

					continue;
				}

				// Load Normals
				if ( line.substr( 0, 3 ) == "vn " )
				{
					std::istringstream v( line.substr( 2 ) );

					XMFLOAT3 normal;
					double x, y, z;
					v >> x;
					v >> y;
					v >> z;

					normal = XMFLOAT3( x, y, z * -1.0f );

					object3D->normals.push_back( normal );

					continue;
				}

				// Load Material names
				if ( ReadMaterialIdName(
					d3d11Device,
					object3D,
					pathModels,
					line,
					materialId,
					materialsList,
					shaderManager, 
					materialType ) )
				{
					materialId = CurrentId;

					continue;
				}

				// read faces
				if ( line.substr( 0, 2 ) == "f " )
				{
					std::istringstream v( line.substr( 2 ) );
					std::vector<std::string> partsLists = split( v.str( ), ' ' );
					std::vector<std::string> partsIndexesA = split( partsLists[ 2 ], '/' );
					std::vector<std::string> partsIndexesB = split( partsLists[ 1 ], '/' );
					std::vector<std::string> partsIndexesC = split( partsLists[ 0 ], '/' );

					int a, b, c;

					// the first part is the vert index on the triangle vertex
					a = std::stoi( partsIndexesA[ 0 ] );
					b = std::stoi( partsIndexesB[ 0 ] );
					c = std::stoi( partsIndexesC[ 0 ] );

					a--; b--; c--;

					int ua, ub, uc;

					// the second part is the uv index on the triangle vertex
					ua = std::stoi( partsIndexesA[ 1 ] );
					ub = std::stoi( partsIndexesB[ 1 ] );
					uc = std::stoi( partsIndexesC[ 1 ] );

					ua--; ub--; uc--;

					int na, nb, nc;

					// the third part is the normal index on the triangle vertex
					na = std::stoi( partsIndexesA[ 2 ] );
					nb = std::stoi( partsIndexesB[ 2 ] );
					nc = std::stoi( partsIndexesC[ 2 ] );

					na--; nb--; nc--;

					DXFace newFace = DXFace(
						a, b, c,
						na, nb, nc,
						ua, ub, uc,
						materialId );


					object3D->Faces.push_back( newFace );
				}
			}
		}

		return true;
	}
	catch ( int e )
	{
		sprintf( "Unable to open Model: %s\n", fileName.c_str( ) );

		return false;
	}
}