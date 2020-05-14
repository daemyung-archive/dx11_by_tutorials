///////////////////////////////////////////////////////////////////////////////
// Filename: MultiTexture.vs
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
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

///////////////////////////////////////////////////////////////////////////////
// Vertex Shader
///////////////////////////////////////////////////////////////////////////////
PixelInputType MultiTextureVertexShader(VertexInputType input)
{
	PixelInputType output = (PixelInputType)0;


	// To calculate correctly set w to 1.0
	input.position.w = 1.0f;

	// Calculate the position with world, view and projection matrix
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Pass the texture coordinate to pixel shader
	output.tex = input.tex;

	return output;
}
