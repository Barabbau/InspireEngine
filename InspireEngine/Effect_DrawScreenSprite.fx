
cbuffer cbParameters		: register( b0 )
{
	float4x4	worldMatrix;
};

Texture2D		SpriteMap	: register( t0 );
SamplerState	samLinear	: register( s0 );

struct PS_INPUT
{
	float4		Position	: POSITION;
};

struct VS_OUT
{
	float4		Position	: SV_POSITION;
};

VS_OUT VS( PS_INPUT input )
{
	VS_OUT	output;

	output.Position = mul( input.Position, worldMatrix );

	return				output;
}

float4 PS( VS_OUT input )	: SV_Target0
{
	float2	ShadowTexCoord	= float2( 0, 0 );

	ShadowTexCoord.x		= ( 0.5 * input.Position.x / input.Position.w + 0.5 ) / 683.0; // Half screen size
	ShadowTexCoord.y		= ( 0.5 * input.Position.y / input.Position.w + 0.5 ) / 384.0;

	float4	addColor		= SpriteMap.Sample( samLinear, ShadowTexCoord );
	
	return	float4( addColor.r, 0, 0, addColor.r );
}

