#pragma once

#include "Mesh.h"

Mesh::Mesh( )
{
	//ctor
}

Mesh::~Mesh( )
{
	//ctor
}

/// <summary>
/// Calculate Mesh Tangents
/// </summary>
void Mesh::ComputeTangents( )
{
	this->tangents = std::vector<XMFLOAT3>( this->vertices.size( ), XMFLOAT3(0, 0, 0 ) );
	this->binormals = std::vector<XMFLOAT3>( this->vertices.size( ), XMFLOAT3( 0, 0, 0 ) );

	/*
	// Speed up math by copying the mesh arrays.
	std::vector<INT32> triangles;

	// Speed up math by copying the mesh arrays.
	for ( size_t i = 0; i < this->Faces.size( ); i++ )
	{
		const DXFace &face = this->Faces.at( i );

		triangles.push_back( face.indexes[ 0 ] );
		triangles.push_back( face.indexes[ 1 ] );
		triangles.push_back( face.indexes[ 2 ] );
	}
	
	if ( this->vertices.size() == 0 || this->uvs.size( ) == 0 || this->normals.size( ) == 0 )
	{
		return;
	}

	// Variable Definitions
	int triangleCount = this->Faces.size( ) * 3;
	int vertexCount = this->vertices.size( );

	std::vector<XMFLOAT3> tan1;
	std::vector<XMFLOAT3>  tan2;
	std::vector<XMFLOAT4> newTangets;

	for ( INT a = 0; a < vertexCount; a++ )
	{
		XMFLOAT3 zeroValue = XMFLOAT3( 0, 0, 0 );

		tan1.push_back( zeroValue );
		tan2.push_back( zeroValue );

		newTangets.push_back( XMFLOAT4( 0, 0, 0, 0 ) );
	}

	for ( size_t i = 0; i < this->Faces.size( ); i++ )
	{
		const DXFace &face = this->Faces.at( i );

		XMFLOAT3 v1 = this->vertices.at( face.indexes[ 0 ] );
		XMFLOAT3 v2 = this->vertices.at( face.indexes[ 1 ] );
		XMFLOAT3 v3 = this->vertices.at( face.indexes[ 2 ] );

		XMFLOAT2 w1 = this->uvs.at( face.uvIndexes[ 0 ]);
		XMFLOAT2 w2 = this->uvs.at( face.uvIndexes[ 1 ] );
		XMFLOAT2 w3 = this->uvs.at( face.uvIndexes[ 2 ] );

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float div = ( s1 * t2 ) - ( s2 * t1 );
		float r = abs( div ) < 0.001f ? 0.0f : 1.0f / div;

		XMFLOAT3 sdir = XMFLOAT3( ( ( t2 * x1 ) - ( t1 * x2 ) ) * r, ( ( t2 * y1 ) - ( t1 * y2 ) ) * r, ( ( t2 * z1 ) - ( t1 * z2 ) ) * r );
		XMFLOAT3 tdir = XMFLOAT3( ( ( s1 * x2 ) - ( s2 * x1 ) ) * r, ( ( s1 * y2 ) - ( s2 * y1 ) ) * r, ( ( s1 * z2 ) - ( s2 * z1 ) ) * r );

		tan1.at( face.indexes[ 0 ] ).x += sdir.x;
		tan1.at( face.indexes[ 0 ] ).y += sdir.y;
		tan1.at( face.indexes[ 0 ] ).z += sdir.z;

		tan1.at( face.indexes[ 1 ] ).x += sdir.x;
		tan1.at( face.indexes[ 1 ] ).y += sdir.y;
		tan1.at( face.indexes[ 1 ] ).z += sdir.z;

		tan1.at( face.indexes[ 2 ] ).x += sdir.x;
		tan1.at( face.indexes[ 2 ] ).y += sdir.y;
		tan1.at( face.indexes[ 2 ] ).z += sdir.z;

		tan2.at( face.indexes[ 0 ] ).x += tdir.x;
		tan2.at( face.indexes[ 0 ] ).y += tdir.y;
		tan2.at( face.indexes[ 0 ] ).z += tdir.z;

		tan2.at( face.indexes[ 1 ] ).x += tdir.x;
		tan2.at( face.indexes[ 1 ] ).y += tdir.y;
		tan2.at( face.indexes[ 1 ] ).z += tdir.z;

		tan2.at( face.indexes[ 2 ] ).x += tdir.x;
		tan2.at( face.indexes[ 2 ] ).y += tdir.y;
		tan2.at( face.indexes[ 2 ] ).z += tdir.z;
	//}

		for ( INT a = 0; a < 3; ++a )
		{
			XMVECTOR n = XMLoadFloat3( &this->normals.at( a ) );
			XMVECTOR t = XMLoadFloat3( &tan1.at( a ) );

			XMVECTOR tmp = XMVector3Normalize( t - ( n * XMVector3Dot( n, t ) ) );

			//XMFLOAT3 zero = XMFLOAT3( 0, 0, 0 );
			//float w = XMVector3GreaterOrEqual( XMVector3Dot( XMVector3Cross( n, t ), XMLoadFloat3( &tan2.at( a ) ) ), XMLoadFloat3( &zero ) ) ? 1.0f : -1.0f;
			this->tangents.at( face.indexes[ a ] ) = XMFLOAT3(
				XMVectorGetByIndex( tmp, 0 ),
				XMVectorGetByIndex( tmp, 1 ),
				XMVectorGetByIndex( tmp, 2 )
			);
		}
	}

	for ( size_t i = 0; i < this->tangents.size( ); i++ )
	{
		XMVECTOR vector = XMLoadFloat3( &this->tangents[ i ] );
		vector = XMVector3Normalize( vector );
		XMFLOAT3 normalizedValue;
		XMStoreFloat3( &normalizedValue, vector );
		this->tangents[ i ] = normalizedValue;
	}
	*/
	
	for ( size_t i = 0; i < this->Faces.size( ); i++ )
	{
		const DXFace &face = this->Faces.at( i );

		for ( size_t b = 0; b < 3; b += 3 )
		{
			XMFLOAT3 vertex0 = this->vertices.at( face.indexes[ b ] );
			XMFLOAT3 vertex1 = this->vertices.at( face.indexes[ b + 1 ] );
			XMFLOAT3 vertex2 = this->vertices.at( face.indexes[ b + 2 ] );

			// Edges of the triangle : position delta
			XMVECTOR deltaPos1 = XMLoadFloat3( &vertex1 ) - XMLoadFloat3( &vertex0 );
			XMVECTOR deltaPos2 = XMLoadFloat3( &vertex2 ) - XMLoadFloat3( &vertex0 );

			// Shortcuts for UVs
			XMFLOAT2 uv0 = this->uvs.at( face.uvIndexes[ b ] );
			XMFLOAT2 uv1 = this->uvs.at( face.uvIndexes[ b + 1 ] );
			XMFLOAT2 uv2 = this->uvs.at( face.uvIndexes[ b + 2 ] );

			// Shortcuts for UVs
			XMVECTOR deltaUV1 = XMLoadFloat2( &uv1 ) - XMLoadFloat2( &uv0 );
			XMVECTOR deltaUV2 = XMLoadFloat2( &uv2 ) - XMLoadFloat2( &uv0 );

			// We can now use our formula to compute the tangent and the bitangent
			XMFLOAT2 deltaUV1F;
			XMStoreFloat2( &deltaUV1F, deltaUV1 );
			XMFLOAT2 deltaUV2F;
			XMStoreFloat2( &deltaUV2F, deltaUV2 );

			float r = 1.0f / ( deltaUV1F.x * deltaUV2F.y - deltaUV1F.y * deltaUV2F.x );
			XMVECTOR tangent= ( deltaPos1 * deltaUV2F.y - deltaPos2 * deltaUV1F.y ) * r;
			XMVECTOR binormal = ( deltaPos2 * deltaUV1F.x - deltaPos1 * deltaUV1F.x ) * r;

			XMVECTOR normal = XMVector3Normalize( XMLoadFloat3( &this->normals.at( face.normalIndexes[ b ] ) )
								+ XMLoadFloat3( &this->normals.at( face.normalIndexes[ b + 1 ] ) )
								+ XMLoadFloat3( &this->normals.at( face.normalIndexes[ b + 2 ] ) ) );


			XMFLOAT3 tangentF;
			XMStoreFloat3( &tangentF, tangent );

			tangent = XMVector3Normalize( tangent - XMVector3Dot( tangent, normal ) * normal );
			for ( size_t nIndex = 0; nIndex < 3; nIndex++ )
			{
				this->tangents.at( face.normalIndexes[ b + nIndex ] ) = tangentF;
			}

			XMFLOAT3 binormalF;
			XMStoreFloat3( &binormalF, binormal );
			for ( size_t nIndex = 0; nIndex < 3; nIndex++ )
			{
				this->binormals.at( face.normalIndexes[ b + nIndex ] ) = binormalF;
			}			
		}
	}

	for ( size_t i = 0; i < this->tangents.size( ); i++ )
	{
		XMVECTOR vector = XMLoadFloat3( &this->tangents[ i ] );
		vector = XMVector3Normalize( vector );
		XMFLOAT3 normalizedValue;
		XMStoreFloat3( &normalizedValue, vector );
		this->tangents[ i ] = normalizedValue;

		XMVECTOR vectorBi = XMLoadFloat3( &this->binormals[ i ] );
		vectorBi = XMVector3Normalize( vectorBi );
		XMFLOAT3 normalizedValueBi;
		XMStoreFloat3( &normalizedValueBi, vectorBi );
		this->binormals[ i ] = normalizedValueBi;
	}
}

bool Mesh::ComputeNormals( )
{
	HRESULT hr = 0;

	//////////////////////Compute Normals///////////////////////////
	//If computeNormals was set to true then we will create our own
	//normals, if it was set to false we will use the obj files normals
	{
		std::vector<XMFLOAT3> tempNormal;

		//normalized and unnormalized normals
		XMFLOAT3 unnormalized = XMFLOAT3( 0.0f, 0.0f, 0.0f );

		//Used to get vectors (sides) from the position of the verts
		float vecX, vecY, vecZ;

		//Two edges of our triangle
		XMVECTOR edge1 = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
		XMVECTOR edge2 = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );

		//Compute face normals
		for ( int i = 0; i < this->Faces.size( ); ++i )
		{
			//Get the vector describing one edge of our triangle (edge 0,2)
			vecX = this->vertices[ this->Faces[ i ].indexes[ 0 ] ].x - this->vertices[ this->Faces[ i ].indexes[ 2 ] ].x;
			vecY = this->vertices[ this->Faces[ i ].indexes[ 0 ] ].y - this->vertices[ this->Faces[ i ].indexes[ 2 ] ].y;
			vecZ = this->vertices[ this->Faces[ i ].indexes[ 0 ] ].z - this->vertices[ this->Faces[ i ].indexes[ 2 ] ].z;
			edge1 = XMVectorSet( vecX, vecY, vecZ, 0.0f );	//Create our first edge

			//Get the vector describing another edge of our triangle (edge 2,1)
			vecX = this->vertices[ this->Faces[ i ].indexes[ 0 ] ].x - this->vertices[ this->Faces[ i ].indexes[ 1 ] ].x;
			vecY = this->vertices[ this->Faces[ i ].indexes[ 0 ] ].y - this->vertices[ this->Faces[ i ].indexes[ 1 ] ].y;
			vecZ = this->vertices[ this->Faces[ i ].indexes[ 0 ] ].z - this->vertices[ this->Faces[ i ].indexes[ 1 ] ].z;
			edge2 = XMVectorSet( vecX, vecY, vecZ, 0.0f );	//Create our second edge

			//Cross multiply the two edge vectors to get the un-normalized face normal
			XMStoreFloat3( &unnormalized, XMVector3Cross( edge1, edge2 ) );
			tempNormal.push_back( unnormalized );			//Save unormalized normal (for normal averaging)
		}

		//Compute vertex normals (normal Averaging)
		XMVECTOR normalSum = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
		int facesUsing = 0;
		float tX;
		float tY;
		float tZ;

		this->normals.clear( );

		//Go through each vertex
		for ( int i = 0; i < this->vertices.size( ); ++i )
		{
			//Check which triangles use this vertex
			for ( int j = 0; j < this->Faces.size( ); ++j )
			{
				if ( this->Faces[ j ].indexes[ 0 ] == i ||
					 this->Faces[ j ].indexes[ 1 ] == i ||
					 this->Faces[ j ].indexes[ 2 ] == i )
				{
					tX = XMVectorGetX( normalSum ) + tempNormal[ j ].x;
					tY = XMVectorGetY( normalSum ) + tempNormal[ j ].y;
					tZ = XMVectorGetZ( normalSum ) + tempNormal[ j ].z;

					normalSum = XMVectorSet( tX, tY, tZ, 0.0f );	//If a face is using the vertex, add the unormalized face normal to the normalSum
					facesUsing++;
				}
			}

			//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
			normalSum = normalSum / facesUsing;

			//Normalize the normalSum vector
			normalSum = XMVector3Normalize( normalSum );

			

			//Store the normal in our current vertex
			this->normals.push_back( XMFLOAT3( XMVectorGetX( normalSum ), XMVectorGetY( normalSum ), XMVectorGetZ( normalSum ) ) );

			//Clear normalSum and facesUsing for next vertex
			normalSum = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
			facesUsing = 0;
		}
	}

	return true;
}
/*

		/// <summary>

		/// Save the Object as OBJ

		/// </summary>

		/// <param name="object3D"></param>

		/// <param name="fileName"></param>

		/// <param name="pathModels">models path</param>

		/// <returns>True if file is written successfully</returns>

		public static Boolean Write(Object3D object3D, String fileName, String pathModels)

		{

			// Get non-case sensitive material name

			String objName = Path.Combine(pathModels, fileName);



			try

			{

				using (var file = new StreamWriter(objName))

				{

					// HEADER

					file.WriteLine("# Terrain Editor Wavefront OBJ Exporter v0.01 - (c)2018 RAGE");

					file.WriteLine("# File Created: 04.03.2018 22:07:18");

					file.WriteLine("# ");

					file.WriteLine("# object" + fileName);

					file.WriteLine("# ");

					file.WriteLine("");



					// Vertexes

					foreach (Point point in object3D.Points)

					{

						var line = "v ";

						line += -point[0] + " " + point[1] + " " + point[2];

						file.WriteLine(line);

					}



					var lineCount = "# ";

					lineCount += object3D.Points.Count + "vertices";

					file.WriteLine(lineCount);

					file.WriteLine("");



					// Normals

					foreach (Point normal in object3D.Normals)

					{

						var line = "vn ";

						line += -normal[0] + " " + normal[1] + " " + normal[2];

						file.WriteLine(line);

					}



					var lineCountN = "# ";

					lineCountN += object3D.Normals.Count + "vertex normals";

					file.WriteLine(lineCountN);

					file.WriteLine("");





					// UVs

					foreach (Point2 uv in object3D.UVs)

					{

						var line = "vt ";

						line += uv[0] + " " + (1.0f - uv[1]);

						file.WriteLine(line);

					}



					var lineCountU = "# ";

					lineCountU += object3D.UVs.Count + "texture coords";

					file.WriteLine(lineCountU);

					file.WriteLine("");



					// MESH FACES

					var objNameOut = "g ";

					objNameOut += fileName;

					file.WriteLine(objNameOut);



					Int32 actualMatIndex = -1;



					foreach (ObjFace face in object3D.Faces)

					{

						if (face.Id != actualMatIndex)

						{

							actualMatIndex = face.Id;

							var matNameOut = "usemtl ";

							matNameOut += objNameOut.Substring(0, fileName.Length - 4);

							file.WriteLine(matNameOut);

							file.WriteLine("s 1");

						}



						var faceLine = "f ";

						faceLine +=

							face.Vertices[0].IndexPoint + 1 + "/" + (face.Vertices[0].IndexUv + 1) + "/" + (face.Vertices[0].IndexNormal + 1) + " " +

							(face.Vertices[1].IndexPoint + 1) + "/" + (face.Vertices[1].IndexUv + 1) + "/" + (face.Vertices[1].IndexNormal + 1) + " " +

							(face.Vertices[2].IndexPoint + 1) + "/" + (face.Vertices[2].IndexUv + 1) + "/" + (face.Vertices[2].IndexNormal + 1);



						file.WriteLine(faceLine);

					}



					var lineCountF = "# ";

					lineCountF += object3D.Faces.Count + "faces";

					file.WriteLine(lineCountF);

					file.WriteLine("");

				}

			}

			catch (Exception e)

			{

				var output = "Unable to write Model : ";

				output += fileName;



				ApplicationUtilities.Log.Error(output);

				ApplicationUtilities.Log.Error(e.Message);

				return false;

			}



			return true;

		}




*/