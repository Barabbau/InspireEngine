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

cbuffer cbPerFrame  : register( b0 )
{
	Light light;
};

cbuffer cbPerObject : register( b1 )
{
	float4x4 WVP;
    float4x4 World;
	float4 difColor;
	bool hasTexture;
};

cbuffer cbPerObjectInstanced : register( b2 )
{
	float4x4 WVPs;
	float4x4 Worlds[ 512 ];
	float4 difColorInstanced;
	bool hasTextureInstanced;
};

Texture2D ObjTexture  : register( t0 );
Texture2D ObjNormal  : register( t1 );
SamplerState ObjSamplerState;
TextureCube SkyMap;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 TexCoord : TEXCOORD1;
	float3x3    WorldToTangentSpace    : TEXCOORD2;
};

struct SKYMAP_VS_OUTPUT	//output structure for skymap vertex shader
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

VS_OUTPUT VS( float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT )
{
    VS_OUTPUT output;

    output.Pos = mul( inPos, WVP );
	output.worldPos = mul( inPos, World );

	float3 binormal = normalize( cross( normal, tangent ) );
	output.WorldToTangentSpace[ 0 ] = normalize( mul( tangent, World ) );
	output.WorldToTangentSpace[ 1 ] = normalize( mul( binormal, World ) );
	output.WorldToTangentSpace[ 2 ] = normalize( mul( normal, World ) );

    output.TexCoord = inTexCoord;

    return output;
}

VS_OUTPUT VS_Instanced( float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT, uint instanceID : SV_InstanceID )
{
	VS_OUTPUT output;

	output.Pos = mul( inPos, mul(Worlds[ instanceID ], WVPs ));
	output.worldPos = mul( inPos, Worlds[ instanceID ] );

	float3 binormal = normalize( cross( normal, tangent ) );
	output.WorldToTangentSpace[ 0 ] = normalize( mul( tangent, Worlds[ instanceID ] ) );
	output.WorldToTangentSpace[ 1 ] = normalize( mul( binormal, Worlds[ instanceID ] ) );
	output.WorldToTangentSpace[ 2 ] = normalize( mul( normal, Worlds[ instanceID ] ) );

	output.TexCoord = inTexCoord;

	return output;
}

float4 ComputePS( VS_OUTPUT input, bool isTextured, float4 tintColor )
{
	

	//Set diffuse color of material
	float4 diffuse = tintColor;
	float3 SkyColor = float3( 1.0f, 1.0f, 1.0f );
	float3 bumpNormal = input.WorldToTangentSpace[ 2 ];

	float3 finalColor = float3( 0.0f, 0.0f, 0.0f );

	//Create the vector between light position and pixels position
	float3 lightToPixelVec =  input.worldPos - light.pos;
	float3 vecWorldProjection = normalize( lightToPixelVec );

	if ( isTextured == true )
	{
		// Albedo
		diffuse = ObjTexture.Sample( ObjSamplerState, input.TexCoord );
		clip( diffuse.a < 0.1f ? -1 : 1 );

		// Color Saturation
		float saturationValue = 1.3f;
		float3  LuminanceWeights = float3( 0.299f, 0.587f, 0.114f );

		float    luminance = dot( diffuse, LuminanceWeights );
		diffuse = lerp( luminance, diffuse, float4( saturationValue, saturationValue, saturationValue, 1.0f ) );

		// Sample the pixel in the bump map.
		float3 textureNormal = ObjNormal.Sample( ObjSamplerState, input.TexCoord).xyz; //4 * ( 1.0f - diffuse.g ) ).xyz;
		float g = textureNormal.b;

		textureNormal = ( ( textureNormal * 2.0f ) - 1.0f );
		textureNormal.b = -g;
		textureNormal = normalize( textureNormal );

		// Move to tangentspace
		bumpNormal = normalize( mul( textureNormal, input.WorldToTangentSpace ) );

		// Reflect and fip z to sample cubemap
		float3 cubeSampCoords = reflect( vecWorldProjection, bumpNormal ); //input.WorldToTangentSpace[ 2 ] );// Normal are reblended with model normals
		cubeSampCoords.z = -cubeSampCoords.z;

		SkyColor = SkyMap.Sample( ObjSamplerState, cubeSampCoords)  * ( 1.0 - diffuse.r ) * 0.3; //, diffuse.r * 6 ).xyz * (1 - diffuse.r);
		//return float4( SkyColor, 1.0f );

		//finalColor += SkyColor;
	}

	//Find the distance between the light pos and pixel pos
	float d = length( lightToPixelVec );

	//Add the ambient light
	float3 finalAmbient = diffuse * light.ambient;

	//use the normalized version
	lightToPixelVec = vecWorldProjection;

	//Calculate how much light the pixel gets by the angle
	//in which the light strikes the pixels surface
	float howMuchLight = dot( lightToPixelVec, bumpNormal );

	//If light is striking the front side of the pixel
	if ( howMuchLight > 0.0f )
	{
		howMuchLight = howMuchLight;
		//Add light to the finalColor of the pixel
		finalColor += ( SkyColor * howMuchLight + diffuse * light.diffuse * howMuchLight );// ( ( SkyColor * diffuse * light.diffuse ) * ( 2 * howMuchLight) );

		//Calculate Light's Distance Falloff factor
		finalColor /= ( light.att[ 0 ] + ( light.att[ 1 ] * d ) ) + ( light.att[ 2 ] * ( d * d ) );

		//Calculate falloff from center to edge of pointlight cone
		finalColor *= pow( max( dot( lightToPixelVec, light.dir ), 0.0f ), light.cone );
	}

	//make sure the values are between 1 and 0, and add the ambient
	finalColor = saturate( finalColor + finalAmbient );

	//Return Final Color
	return float4( finalColor, diffuse.a );
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
	return ComputePS( input, hasTexture, difColor );
}

float4 PS_Instanced( VS_OUTPUT input ) : SV_TARGET
{
	return ComputePS( input, hasTextureInstanced, difColorInstanced );
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