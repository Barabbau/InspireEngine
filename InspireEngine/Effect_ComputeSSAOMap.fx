
cbuffer cbChangesEveryFrame			: register( b0 )
{
	float4x4	worldMatrix;
	float4		sampleRadius;
	float4		distanceScale;
};

Texture2D		myDepthMap			: register( t0 );
Texture2D		randomNoiseMap		: register( t1 );
SamplerState	samLinear			: register( s0 );

struct VS_INPUT
{
	float4		Position			: POSITION0;
};

struct PS_INPUT
{
	float4		pos					: SV_POSITION;
	float2		TexCoord			: TEXCOORD0;
};


// Transforms the model into light space and outputs the depth of the object
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT Out;

	Out.pos = mul( input.Position, worldMatrix );

	float4 computeSSAOPosition	= input.Position;
	computeSSAOPosition.xy		= sign(computeSSAOPosition.xy );

	Out.TexCoord.x				= (0.5 * computeSSAOPosition.x / computeSSAOPosition.w + 0.5) / 0.5;
	Out.TexCoord.y				= (0.5 * computeSSAOPosition.y / computeSSAOPosition.w + 0.5) / 0.5;

    return Out;
}

float rand( float2 co )
{
	return frac( sin( dot( co.xy, float2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

// Saves the depth value out to the 32bit floating point texture
float4 PS( PS_INPUT input )		: SV_Target0
{
	//return float4( 1, 1, 0, 0.6f);
	// Constants
	int		iterations			= 10;

	float	finalColor			= 0.0f;
	float	sampleDepth			= 0;
	float	distance			= 0;
	float	maxdist = sampleRadius.g;
	float nearClip = distanceScale.g;
	float farClip = distanceScale.b;


	float4	samples[ 10 ] =
	{
		float4( 0.355512, 	-0.709318, 	-0.102371,	0.0 ),
		float4( 0.534186, 	0.71511, 	-0.115167,	0.0 ),
		float4( -0.87866, 	0.157139, 	-0.115167,	0.0 ),
		float4( 0.140679, 	-0.475516, 	-0.0639818,	0.0 ),
		float4( -0.0796121, 0.158842, 	-0.677075,	0.0 ),
		float4( -0.0759516, -0.101676, 	-0.483625,	0.0 ),
		float4( 0.12493, 	-0.0223423,	-0.483625,	0.0 ),
		float4( -0.0720074, 0.243395, 	-0.967251,	0.0 ),
		float4( -0.207641, 	0.414286, 	0.187755,	0.0 ),
		float4( -0.277332, 	-0.371262, 	0.187755,	0.0 ),

		//float4(0.63864, 	-0.114214, 	0.262857,	0.0),
		//float4(-0.184051, 	0.622119, 	0.262857,	0.0),
		//float4(0.110007, 	-0.219486, 	0.435574,	0.0),
		//float4(0.235085, 	0.314707, 	0.696918,	0.0),
		//float4(-0.290012, 	0.0518654, 	0.522688,	0.0),
		//float4(0.0975089, 	-0.329594, 	0.609803,	0.0)
	};

	float	depth = myDepthMap.Sample( samLinear, input.TexCoord ).r;
	depth /= farClip;
	depth = 1.0 - depth;

	float2	normalTexCoord = input.TexCoord.xy * float2( 200.0, 200.0 );
	float3	randNormal = randomNoiseMap.Sample( samLinear, normalTexCoord ).rgb;

	for ( int i = 0; i < iterations; i++ )
	{
		float3	ray = reflect( samples[ i ].xyz, randNormal ) * sampleRadius.x;

		input.TexCoord.x += ray * ( 0.75 - input.TexCoord.x * 0.5 ) * 2;
		input.TexCoord.y += ray * ( 0.75 - input.TexCoord.y * 0.5 ) * 2;

		//input.TexCoord = saturate( input.TexCoord );

		sampleDepth	= myDepthMap.Sample( samLinear, input.TexCoord ).r;
		sampleDepth /= farClip;
		sampleDepth	= 1.0 - sampleDepth;

		if ( sampleDepth < nearClip )
		{
			return						float4( 0, 0, 0, 1.0 );
		}

		distance				= depth - sampleDepth;

		float	step			= abs( distance );

		if ( sampleDepth == 1.0 )// || step > maxdist )
		{
			finalColor++;
		}
		else
		{
			float	sstep = 1.0 - smoothstep( 0.0, maxdist, step );
			float	occlusion = sstep * distanceScale.r * max( -distance, 0.0f );

			finalColor += ( 1.0f / ( 1.0f + occlusion * occlusion ) );
		}
	}

	float	greyScaleColor		= clamp( ( finalColor / iterations ), 0.0 ,1.0 );

	return						float4( 0, 0, 0, 1.0 - greyScaleColor );
}
