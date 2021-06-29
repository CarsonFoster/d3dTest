struct VSOut {
	float4 color : Color;
	float4 pos : SV_Position;
};

VSOut main(float2 pos : Position, float4 color : Color) {
	VSOut output;
	output.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
	output.color = color;
	return output;
}