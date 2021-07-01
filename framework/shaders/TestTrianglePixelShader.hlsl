cbuffer CBuf {
	float4 colors[6];
};

float4 main(uint tId : SV_PrimitiveID) : SV_Target{
	return colors[tId / 2];
}