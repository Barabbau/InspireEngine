
struct Light
{
	float3 pos;
	float  range;
	float3 dir;
	float cone;
	float3 att;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame
{
	Light light;
};

cbuffer cbPerObject
{
	float4x4 WVP;
    float4x4 World;

	float4 difColor;
	bool hasTexture;
};

Texture2D ObjTexture  : register( t0 );
Texture2D ObjNormal  : register( t1 );
SamplerState ObjSamplerState;
TextureCube SkyMap;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct SKYMAP_VS_OUTPUT	//output structure for skymap vertex shader
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
    VS_OUTPUT output;

    output.Pos = mul(inPos, WVP);
	output.worldPos = mul(inPos, World);

	output.normal = mul(normal, World);

    output.TexCoord = inTexCoord;

    return output;
}


SKYMAP_VS_OUTPUT SKYMAP_VS(float3 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	output.Pos = mul(float4(inPos, 1.0f), WVP).xyww;

	output.texCoord = inPos;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	//return float4( 1.0f, 1.0f, 1.0f, 1.0f );
	input.normal = normalize(input.normal);	

	//Set diffuse color of material
	float4 diffuse = difColor;
	
	float3 bumpNormal = input.normal.xyz;

	//If material has a diffuse texture map, set it now
	if(hasTexture == true)
	{
		// Sample the pixel in the bump map.
		float3 normal = ObjNormal.Sample( ObjSamplerState, input.TexCoord ).xyz;

		// Expand the range of the normal value from (0, +1) to (-1, +1).
		normal = ( normal * 2.0f ) - 1.0f;

		float3 tangent;

		float3 c1 = cross( input.normal, float3( 0.0, 0.0, 1.0 ) );
		float3 c2 = cross( input.normal, float3( 0.0, 1.0, 0.0 ) );

		if ( length( c1 ) > length( c2 ) )
		{
			tangent = c1;
		}
		else
		{
			tangent = c2;
		}
		//tangent = float3( ddx( input.TexCoord.x ), 0.0f, ddx( input.TexCoord.y ) );
		//tangent = normalize( tangent );
		float3 binormal = cross( input.normal, tangent.xyz );



		// Calculate the normal from the data in the bump map.
		bumpNormal = ( normal.x * tangent ) + ( normal.y * binormal ) + ( normal.z * input.normal );

		//normal = mul( normal, World );
		diffuse = ObjTexture.Sample( ObjSamplerState, input.TexCoord );
	}



//	float3 bumpNormal = normalize (input.normal + normal);// ( normal.x * input.normal + normal.z * tangent + normal.x * binormal );// normal.z * input.normal + ( normal.x * tangent + normal.y * binormal ) );
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	
	//Create the vector between light position and pixels position
	float3 lightToPixelVec = light.pos - input.worldPos;
		
	//Find the distance between the light pos and pixel pos
	float d = length(lightToPixelVec);
	
	//Add the ambient light
	float3 finalAmbient = diffuse * light.ambient;

	//If pixel is too far, return pixel color with ambient light
	if( d > light.range )
		return float4(finalAmbient, diffuse.a);
		
	//Turn lightToPixelVec into a unit length vector describing
	//the pixels direction from the lights position
	lightToPixelVec /= d; 
		
	//Calculate how much light the pixel gets by the angle
	//in which the light strikes the pixels surface
	float howMuchLight = dot(lightToPixelVec, bumpNormal );

	//If light is striking the front side of the pixel
	if( howMuchLight > 0.0f )
	{	
		//Add light to the finalColor of the pixel
		finalColor += diffuse * light.diffuse * howMuchLight;
					
		//Calculate Light's Distance Falloff factor
		finalColor /= (light.att[0] + (light.att[1] * d)) + (light.att[2] * (d*d));		

		//Calculate falloff from center to edge of pointlight cone
		finalColor *= pow(max(dot(-lightToPixelVec, light.dir), 0.0f), light.cone);
	}
	
	//make sure the values are between 1 and 0, and add the ambient
	finalColor = saturate(finalColor * 1.4f + finalAmbient * 1.4f );
	
	//Return Final Color
	return float4(finalColor, diffuse.a);
}

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
	return SkyMap.Sample(ObjSamplerState, input.texCoord);
}

float4 D2D_PS(VS_OUTPUT input) : SV_TARGET
{
    float4 diffuse = ObjTexture.Sample( ObjSamplerState, input.TexCoord );
	
	return diffuse;
}
