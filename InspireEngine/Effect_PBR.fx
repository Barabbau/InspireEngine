
cbuffer cbNeverChanges					: register( b0 )
{
	float3		LightDirection;
};

cbuffer cbChangesEveryFrame				: register( b1 )
{
	//matrix
	float		LightIntensity;
	float4		LightPosition			= float4( 300.0f, 300.0f, -300.0f, 0.0f );
	float4		EyePosition				= float4( 0.0f, 100.0f, -300.0f, 1.0f );
	matrix		World;
	matrix		View;
	matrix		Projection;
	matrix		LightViewProj;
};


Texture2D		ColorMap				: register( t0 );
Texture2D		NormalMap				: register( t1 );
Texture2D		MaterialMap				: register( t2 );
Texture2D		ShadowMap				: register( t3 );

TextureCube		TexCubemapSpecular		: register( t4 );
TextureCube		TexCubemapIrradiance	: register( t5 );

SamplerState	samLinear				: register( s0 );
SamplerState	samPoint				: register( s1 );

SamplerState CubeSamplerSampler
{
	Filter		= MIN_MAG_MIP_LINEAR;
	AddressU	= Wrap;
	AddressV	= Wrap;
};


struct VS_INPUT
{
	float4		Position			: POSITION0;
	float2		TexCoord			: TEXCOORD0;

	float3		Normal				: NORMAL0;
	float3		Binormal			: BINORMAL0;
	float3		Tangent				: TANGENT0;
};

struct PS_INPUT
{
	float4		Position			: SV_POSITION;
	float2		TexCoord			: TEXCOORD0;
	float3		LightVector			: TEXCOORD1;
	float3x3	WorldToTangentSpace	: TEXCOORD2;

	float3		ViewDir				: COLOR1;
	float4		wPosition			: COLOR0;
};


float random(float3 seed, int i)
{
	float4	seed4		= float4(seed, i);
	float	dot_prod	= dot(seed4, float4(12.9898, 78.233, 45.164, 94.673));

	return	frac(sin(dot_prod) * 43758.5453);
}


// Poission blur based on multiple samples
float shadowPoisson(float light_space_depth, float2 shadow_coord)
{
	float2 poisson_disk[ 8 ] = {
		float2( -0.94201624, -0.39906216 ),
		float2( 0.94558609, -0.76890725 ),
		float2( -0.094184101, -0.92938870 ),
		float2( 0.34495938, 0.29387760 ),
		float2( -0.91588581, 0.45771432 ),
		float2( -0.81544232, -0.87912464 ),
		float2( -0.38277543, 0.27676845 ),
		float2( 0.97484398, 0.75648379 ),
	};

	float samples = 8;
	float visibility = 1.0f;

	//samples
	for ( int i = 0; i < samples; i++ )
	{
		visibility -= ( 1 / samples ) * ( light_space_depth < ShadowMap.SampleLevel( samLinear, shadow_coord + poisson_disk[ i ] / 1000, 0 ).r );
	}

	return 1.0 - visibility;
}


// Multisample PCF shadow blur
float calcShadowPCF(float light_space_depth, float2 shadow_coord)
{
	float	shadow_term			= 0;
	float2	shadow_map_size		= float2(1366, 768);// 2048;
	//float2	v_lerps = frac(shadow_map_size * shadow_coord);
	float	bias				= -0.0015f;

	float	sizex				= 1.0 / shadow_map_size.x;
	float	sizey				= 1.0 / shadow_map_size.y;

	float samples[ 4 ];
	samples[ 0 ]				= ( light_space_depth - bias < ShadowMap.Sample( samPoint, shadow_coord ).r );
	samples[ 1 ]				= ( light_space_depth - bias < ShadowMap.Sample( samPoint, shadow_coord + float2( sizex, 0 ) ).r );
	samples[ 2 ]				= ( light_space_depth - bias < ShadowMap.Sample( samPoint, shadow_coord + float2( 0, sizey ) ).r );
	samples[ 3 ]				= ( light_space_depth - bias < ShadowMap.Sample( samPoint, shadow_coord + float2( sizex, sizey ) ).r );

	shadow_term					= ( samples[0] + samples[1] + samples[2] + samples[3] ) / 4.0;
	//shadow_term				= lerp( lerp( samples[ 0 ], samples[ 1 ], v_lerps.x ), lerp( samples[ 2 ], samples[ 3 ], v_lerps.x ), v_lerps.y );

	return shadow_term;
}


PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT							output;

	output.wPosition					= mul( input.Position, World );
	float4 viewPosition					= mul( output.wPosition, View );
	output.Position						= mul( viewPosition, Projection );

	output.TexCoord						= input.TexCoord;

	output.WorldToTangentSpace[ 0 ] = normalize( mul( input.Tangent, World ) );
	output.WorldToTangentSpace[ 1 ] = normalize( mul( input.Binormal, World ) );
	output.WorldToTangentSpace[ 2 ] = normalize( mul( input.Normal, World ) );

	// set view vector and full lenght light vector ( we don't normalize the light vector to use it for light fall off )
	output.ViewDir						= normalize( EyePosition.xyz - output.wPosition.xyz );
	output.LightVector					= ( LightPosition.xyz - output.wPosition.xyz );

	return								output;
}


// remap values from low1/high1 range to low2/high2
float map( float value, float  low1, float  high1, float  low2, float high2 )
{
	return low2 + ( value - low1 ) * ( high2 - low2 ) / ( high1 - low1 );
}


// Lys function
float GetSpecPowToMip( float fSpecPow, int nMips )
{
	// Lys constants
	static const float k0 = 0.00098, k1 = 0.9921, fUserMaxSPow = 0.2425;
	static const float g_fMaxT = ( exp2( -10.0 / sqrt( fUserMaxSPow ) ) - k0 ) / k1;


	fSpecPow = 1 - pow( 1 - fSpecPow, 8 );

	// Default curve - Inverse of Toolbag 2 curve with adjusted constants.
	float fSmulMaxT = ( exp2( -10.0 / sqrt( fSpecPow ) ) - k0 ) / k1;

	return float( nMips - 1 ) * ( 1.0 - clamp( fSmulMaxT / g_fMaxT, 0.0, 1.0 ) );
}


float4 PS( PS_INPUT input ) : SV_Target0
{
	float3	LightDiffuseColor	= float3( 1.0f, 1.0f, 1.0f );
	float	bias				= -1.2655f;

	float	lightMaxDist		= 1000.0f;

	// Sample the textures

	// NORMAL the Tangent/Normal/Binormal are mized with the texture normals to create final normals
	float3  TextureNormal		= normalize( ( 2.0f * NormalMap.Sample( samLinear, input.TexCoord ).xyz ) - 1.0f );
//	float3	Normal				= normalize( input.WorldToTangentSpace[ 2 ] * TextureNormal.b + input.WorldToTangentSpace[ 0 ] * TextureNormal.g + input.WorldToTangentSpace[ 1 ] * TextureNormal.r );
	float3	Normal				= normalize( mul( TextureNormal, input.WorldToTangentSpace ) );

	// DIFFUSE texture comes with Albedo Color + some AO backed for some cases of pre occlusion
	float3  Diffuse				= ColorMap.Sample( samLinear, input.TexCoord ).rgb;

	// ROUGHNESS = Material.r ( how much blurry is the reflection )
	// METALNESS = Material.g ( how much the material reflects )
	// OCCUSION  = Material.b ( how much occlusion )
	float3  Material			= MaterialMap.Sample( samLinear, input.TexCoord ).rgb;

	// Cubemap reflections UVW Coords are created with reflection of world and eye position
	float3 vecWorldProjection	= input.wPosition.xyz - EyePosition.xyz;
	vecWorldProjection			= normalize( vecWorldProjection );
	float3 cubeSampCoords		= reflect( vecWorldProjection , Normal );// Normal are reblended with model normals

	// Cubamep inversion for reflection
	cubeSampCoords.z			= -cubeSampCoords.z;
	float4 cubemapSampleAmbient = TexCubemapSpecular.Sample( samLinear, cubeSampCoords );///= abs(NormalDotLight);
	float4 cubemapSampleSpec	= TexCubemapIrradiance.Sample( samLinear, cubeSampCoords );// *fLighting;


	float luminance				= saturate( ( cubemapSampleSpec.r + cubemapSampleSpec.g + cubemapSampleSpec.b ) / 3.0 );

	// Creating specular color and intensity, this needs to be done before gamma correction
	float3 specularColor		= float3( lerp( 0.04f.rrr, Diffuse.rgb, Material.g ) ) * Material.b;
	Diffuse.rgb					= lerp( Diffuse.rgb, 0.0f.rrr,  luminance * Material.g );

	// Gamma Correction
	cubemapSampleAmbient		= pow( abs( cubemapSampleAmbient ), 2.2 );
	cubemapSampleSpec			= pow( abs( cubemapSampleSpec ), 2.2 );
	Material					= pow( abs( Material ), 2.2 );



	// Normalize input
	float3  vecViewDir			= normalize( input.ViewDir );
	float3  vecLightDir			= normalize( input.LightVector * 0.75 + dot( -TextureNormal, input.LightVector ) * 0.25 );//input.LightVector );
	float3  vecHalf				= normalize( vecLightDir + vecViewDir );

	// Compute Aliases
	float	NormalDotHalf		= dot( Normal, vecHalf );
	float	vecViewDotHalf		= dot( vecHalf,  vecViewDir );
	float	NormalDotView		= dot( Normal, vecViewDir );
	float	NormalDotLight		= dot( Normal, vecLightDir );

	// Compute the geometric term
	float  G1					= ( 2.0f * NormalDotHalf * NormalDotView ) / vecViewDotHalf;
	float  G2					= ( 2.0f * NormalDotHalf * NormalDotLight ) / vecViewDotHalf;
	float  G					= min( 1.0f, max( 0.0f, min( G1, G2 ) ) );

	// Compute the fresnel term
	float  F					= Material.g + ( 1.0f - Material.g ) * pow( 1.0f - NormalDotView, 5.0f );

	// Compute the roughness term
	float	R_2					= Material.r * Material.r;
	float	NDotH_2				= NormalDotHalf * NormalDotHalf;
	float	A					= 1.0f / ( 4.0f * R_2 * NDotH_2 * NDotH_2 );
	float	B					= exp( -( 1.0f - NDotH_2 ) / ( R_2 * NDotH_2 ) );
	float	R					= A * B;

	// Compute the final term
	float3  S					= specularColor * ( G * F * R ) / ( NormalDotLight * NormalDotView );


	// Introduce concept of Point Light Falloff
	float	distance			= length( input.LightVector );

	// Distance cap
	if ( distance > lightMaxDist )distance = lightMaxDist;
	distance					= LightIntensity - map( distance, 0.0, lightMaxDist, 0.0, LightIntensity );

	float	diffuseLighting		= 0.5 + saturate(dot(input.WorldToTangentSpace[2], vecLightDir)) * 0.5;			// per pixel diffuse lighting
	diffuseLighting				*= distance / dot( vecLightDir, vecLightDir );		// Distance from light
	diffuseLighting				= clamp( diffuseLighting, 0.0, 1.0 );

	diffuseLighting				= 1 - cos( diffuseLighting * 1.57 );

	// Composite
	float3  Final				= ( LightDiffuseColor.rgb * diffuseLighting * ( Diffuse + clamp( S, 0, 1 ) ) );


	// Use roughness to mediate between low and high res reflection map ( could be done with mipmaps in Shader Model 4.0 )
	float	mediator			= ( ( GetSpecPowToMip( Material.r, 256 ) ) / 256.0 );
	float3	addCube				= lerp( cubemapSampleAmbient.rgb, cubemapSampleSpec.rgb, mediator );
	addCube						*= saturate( Material.g * 2 );// * 2;

	// add the diffuse ( consider that the Diffuse has already been mediated by the Metalness value before gamma correction )
	addCube						+= saturate( Diffuse * ( lerp( luminance, diffuseLighting, 0.66 ) ) );

	// Add in occlusion
	float occlusion				= 1 - ( 1 - Material.b ) * 0.75f;
	Final						+= addCube * ( Material.b * 0.75f );
	Final						= saturate( Final * occlusion );


	// Find the position of this pixel in light space
	float4 lightingPosition		= mul( input.wPosition, LightViewProj );

	// Find the position in the shadow map for this pixel
	float2 ShadowTexCoord		= 0.5 * lightingPosition.xy / lightingPosition.w + float2( 0.5, 0.5 );
	ShadowTexCoord.y			= 1.0f - ShadowTexCoord.y;

	// Get the current depth stored in the shadow map
	float shadowdepth			= ShadowMap.Sample( samPoint, ShadowTexCoord ).r;

	// Calculate the current pixel depth
	float ourdepth				= ( lightingPosition.z / lightingPosition.w ) - bias;


	// Check to see if this pixel is in front or behind the value in the shadow map
	if ( shadowdepth < ourdepth )
	{
		// Blur the shadows using multiple neighor pixel depths
		float2	screen_pos			= 0.5 + float2( lightingPosition.x, -lightingPosition.y ) * 0.5;
		float	light_space_depth	= lightingPosition.z;
		float	shadow_term			= shadowPoisson( light_space_depth, screen_pos );

		// Shadow the pixel by lowering the intensity
		Final						-= Final * ( 0.15 + ( 1 - saturate( diffuseLighting + shadow_term ) ) * 0.85 );
	};

	float	diffuseIntensity	= saturate( dot( -( LightDirection ), Normal ) );
	Final						= ( Final + ( Final * diffuseIntensity ) );


	return float4( Final.rgb, 1.0f );
}