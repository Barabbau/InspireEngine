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
	DXShaderManager &shaderManager )
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
			std::wstring filePath;
			StringToWString( filePath, filePathStd );

			HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
			&d3d11Device,
			filePath.c_str( ),
			NULL,
			NULL,
			&newMaterial.albedoTexture,
			NULL );


			newMaterial.difColor = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			newMaterial.hasTexture = true;
			const std::string::size_type size = strLineName.size( );
			
			newMaterial.matName = new char[ size + 1 ];  //we need extra char for NUL
			memcpy( newMaterial.matName, strLineName.c_str( ), size + 1 );

			newMaterial.shader = shaderManager._stdShader;
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

		_currentId = materialId;

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
	XMFLOAT3* minVertex,
	XMFLOAT3* maxVertex )
{
	minVertex = &XMFLOAT3( 100000, 100000, 100000 );
	maxVertex = &XMFLOAT3( -100000, -100000, -100000 );

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
			exit( 1 );
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
					shaderManager
				);
			}
			else
			{
				break;
			}
		}

		materialId = _currentId;

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

					vert = XMFLOAT3( -x, y, z );
					object3D->vertices.push_back( vert );

					// Calculate mix and max for the bounding box
					if ( object3D->vertices[ 0 ].x < minVertex->x )
					{
						minVertex->x = object3D->vertices[ 0 ].x;
					}

					if ( object3D->vertices[ 0 ].y < minVertex->y )
					{
						minVertex->y = object3D->vertices[ 0 ].y;
					}

					if ( object3D->vertices[ 0 ].z < minVertex->z )
					{
						minVertex->z = object3D->vertices[ 0 ].z;
					}


					if ( object3D->vertices[ 0 ].x > maxVertex->x )
					{
						maxVertex->x = object3D->vertices[ 0 ].x;
					}

					if ( object3D->vertices[ 0 ].y > maxVertex->y )
					{
						maxVertex->y = object3D->vertices[ 0 ].y;
					}

					if ( object3D->vertices[ 0 ].z > maxVertex->z )
					{
						maxVertex->z = object3D->vertices[ 0 ].z;
					}

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

					uv = XMFLOAT2( x, 1 - y );
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

					normal = XMFLOAT3( x, y, z );
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
					shaderManager ) )
				{
					materialId = _currentId;

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