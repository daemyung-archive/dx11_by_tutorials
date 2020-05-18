///////////////////////////////////////////////////////////////////////////////
// Filename: Texture.vs
///////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

///////////////////////////////////////////////////////////////////////////////
// Vertex Shader
///////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;


	// ??? ?? ??? ?? ?? ??? 4 ??? ?????
	input.position.w = 1.0f;

	// ??, ? ? ?? ??? ?? ??? ??? ?????
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// ?? ???? ??? ?? ??? ?????
	output.tex = input.tex;

	// Calculate normal vector with world matrix
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// Normalize normal vector
	output.normal = normalize(output.normal);

	return output;
}
