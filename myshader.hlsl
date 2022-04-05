struct VOut
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer cbPerObject
{
    float4x4 WVP;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

VOut VShader(float4 position : POSITION, float2 texcoord : TEXCOORD)
{
    VOut output;

    output.position = mul(position, WVP);
    // output.position = position;
    output.texcoord = texcoord;

    return output;
}


float4 PShader(VOut input) : SV_TARGET
{
    return ObjTexture.Sample(ObjSamplerState, input.texcoord);
}