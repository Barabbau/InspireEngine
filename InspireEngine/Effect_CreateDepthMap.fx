
cbuffer cbChangesEveryFrame			: register( b0 )
{
	matrix		WorldViewProjection;
};

struct VS_INPUT
{
	float4		Position			: POSITION0;
};

struct PS_INPUT
{
	float4		Position			: SV_POSITION;
	float		SSAODepth			: TEXCOORD1;
};

// Transforms the model into light space and outputs the depth of the object
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT						Out;

	Out.Position					= mul( input.Position, WorldViewProjection );
	Out.SSAODepth					= Out.Position.z;

	return							Out;
}

// Saves the depth value out to the 32bit floating point texture
float4 PS( PS_INPUT input )			: SV_Target0
{
	return							float4( input.SSAODepth, 0, 0, 0 );
}