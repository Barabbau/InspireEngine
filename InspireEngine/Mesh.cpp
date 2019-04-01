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