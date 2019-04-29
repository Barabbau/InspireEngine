
cbuffer cbChangesEveryFrame			: register( b0 )
{
	matrix		World;
	matrix		LightViewProj;
};

struct VS_INPUT
{
	float4		Position			: POSITION0;
};

struct PS_INPUT
{
	float4		Position			: SV_POSITION;
	float		Depth				: TEXCOORD1;
};

// Transforms the model into light space and outputs the depth of the object
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT						Out;

	float4	worldPosition			= mul( input.Position, World );
	Out.Position					= mul( worldPosition, LightViewProj );
	Out.Depth						= -( Out.Position.z / Out.Position.w );

	return Out;
}

// Saves the depth value out to the 32bit floating point texture
float4 PS( PS_INPUT input )			: SV_Target0
{
	return							float4( input.Depth, 0, 0, 1 );
}
