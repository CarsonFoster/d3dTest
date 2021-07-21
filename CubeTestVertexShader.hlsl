cbuffer Buf {
	matrix transform;
};

struct VSOut {
	float2 tc : TextureCoord;
	float4 pos : SV_Position;
};

VSOut main( float4 pos : Position, float2 tc : TextureCoord )
{
	VSOut output;
	output.tc = tc;
	output.pos = mul(pos, transform);
	return output;
}