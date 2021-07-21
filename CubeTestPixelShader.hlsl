Texture2D tex;
SamplerState smpl;

float4 main(float2 tc : TextureCoord) : SV_TARGET
{
	return tex.Sample(smpl, tc);
}