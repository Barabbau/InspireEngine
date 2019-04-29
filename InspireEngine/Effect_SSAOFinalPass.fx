
texture SSAOTex;

sampler2D SSAOSampler = sampler_state
{
	Texture		= <SSAOTex>;
    ADDRESSU	= CLAMP;
	ADDRESSV	= CLAMP;
	MAGFILTER	= LINEAR;
	MINFILTER	= LINEAR;
};

texture SceneTexture;

sampler2D baseSampler = sampler_state
{
	Texture		= <SceneTexture>;
    ADDRESSU	= CLAMP;
	ADDRESSV	= CLAMP;
	MAGFILTER	= LINEAR;
	MINFILTER	= LINEAR;
};

float HscreenScale	= 1.0 / 2732.0;
float VscreenScale	= 1.0 / 1536.0;

float4 PixelShaderFunction( float2 TexCoord :TEXCOORD0 ) : COLOR0
{
	float4	ColorSample			= tex2D( baseSampler, TexCoord );
	float	OcclusionSample		= tex2D( SSAOSampler, TexCoord ).r;

	TexCoord.x += HscreenScale;
	TexCoord.y += VscreenScale;

	return  ( ColorSample )  - ( 1 - ( OcclusionSample ) );
}


technique Merge
{
    pass Pass1
    {
        PixelShader		= compile ps_3_0 PixelShaderFunction();
    }
}