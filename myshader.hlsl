struct VOut
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct Light
{
    float3 dir;
    float3 pos;
    float  range;
    float3 att;
    float4 ambient;
    float4 diffuse;
};

cbuffer cbPerObject
{
    float4x4 WVP;
    float4x4 World;
};

cbuffer cbPerFrame
{
    Light light;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

VOut VShader(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
    VOut output;

    output.position = mul(position, WVP);

    output.worldPos = mul(position, World);

    output.normal = mul(normal, World);

    output.texcoord = texcoord;

    return output;
}


float4 PShader(VOut input) : SV_TARGET
{
    input.normal = normalize(input.normal);

    float4 diffuse = ObjTexture.Sample( ObjSamplerState, input.texcoord );

    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    if(all(light.dir == float3(0.0f, 0.0f, 0.0f))) {
        //Point Light Version
        //Create the vector between the light position and the pixels position
        float3 lightToPixelVec = light.pos - input.worldPos;

        //Find the distance between the light pos and pixel pos
        float d = length(lightToPixelVec);

        //Create the ambient light
        float3 finalAmbient = diffuse * light.ambient;

        //If the pixel is too far from the light, just return just ambient
        if(d>light.range)
            return float4(finalAmbient, diffuse.a);

        //Turn lightToPixelVec into a unit length vector describing the pixels direction from light to pixel
        lightToPixelVec /= d;

        //Calculate how much light the pixel gets by the angle in which the light strikes the pixels surface
        float howMuchLight = dot(lightToPixelVec, input.normal);
        //If light is triking the front side of the pixel
        if(howMuchLight > 0.0f) {
            //Add light to the final color of the pixel
            finalColor += howMuchLight * diffuse * light.diffuse;

            //Calculate Light's Falloff factor
            finalColor /= light.att[0] + (light.att[1]*d) + (light.att[2]*d*d);
        }
        //Make sure the values are between 1 and 0, and add the ambient
        finalColor = saturate(finalColor + finalAmbient);
    } else {
        //Directional Light Version
        finalColor = diffuse * light.ambient;
        finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);
    }
    //REturn the final color

    return float4(finalColor, diffuse.a);

}