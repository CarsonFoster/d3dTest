cbuffer CBuf {
	float4x4 transform;
};

float4 main(float3 pos : Position) : SV_Position{
	return mul(float4(pos, 1.0f), transform);
}