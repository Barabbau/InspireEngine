
float4	blurDirection;
float4x4 WorldViewProjection;

Texture2D		SSAOTexture;
//SamplerState baseSampler : register(s0);
sampler2D baseSampler = sampler_state
{
	Texture		= <SSAOTexture>;
	ADDRESSU	= CLAMP;
	ADDRESSV	= CLAMP;
	MAGFILTER	= LINEAR;
	MINFILTER	= LINEAR;
};

float HscreenScale	= 1.0 / 2732.0;
float VscreenScale	= 1.0 / 1536.0;

float4 PixelShaderFunction( float2 TexCoord :TEXCOORD0 ) : COLOR0
{
	TexCoord.x				+= HscreenScale;
	TexCoord.y				+= VscreenScale;

    float	color			= tex2D( baseSampler, TexCoord ).r;
   
    float	num				= 1.0;
    int		blurSamples		= 8; 
	
	for( int i = -blurSamples / 2; i <= blurSamples / 2; i++ )
	{
		float4	newTexCoord		= float4( TexCoord + i * blurDirection.xy, 0, 0 );
		float	sample			= tex2D( baseSampler, newTexCoord.xy ).r;
			
		num						+= ( blurSamples / 2 - abs( i ) );
		color					+= sample * ( blurSamples / 2 - abs( i ) );
	}
	
	for( int v = -blurSamples / 2; v <= blurSamples / 2; v++ )
	{
		float4	newTexCoord		= float4( TexCoord + v * blurDirection.zw, 0, 0 );
		float	sample			= tex2D( baseSampler, newTexCoord.xy ).r;
			
		num						+= ( blurSamples / 2 - abs( v ) );
		color					+= sample * ( blurSamples / 2 - abs( v ) );
	}

							// Gamma Correction
	color	= color / num;	//pow ( color / num, 1.2 );
	return float4( color, 1 ,1 ,1 );
}

technique SSAOBlur
{
    pass Pass1
    {
		CullMode		= CCW;
		//VertexShader	= compile vs_4_0 PixelShaderFunction();
        PixelShader		= compile ps_2_0 PixelShaderFunction();
    }
}

