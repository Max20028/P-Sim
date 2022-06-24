struct VOut
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct Light
{
    float3 dir;
    float cone;
    float3 pos;
    float  range;
    float3 att;
    float4 ambient;
    float4 diffuse;
};

cbuffer cbPerFrame
{
    Light light;
};

cbuffer cbPerObject
{
    float4x4 WVP;
    float4x4 World;

    float4 difColor;
    bool hasTexture;
    bool hasNormMap;
};

// Texture2D ObjTexture;
// Texture2D ObjNormMap;
// SamplerState ObjSamplerState;

VOut VShader(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT)
{
    VOut output;

    output.position = mul(position, WVP);

    output.worldPos = mul(position, World);
    return output;
    
    output.normal = mul(normal, World);

    output.texcoord = texcoord;

    output.tangent = mul(tangent, World);

    return output;
}


float4 PShader(VOut input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
    input.normal = normalize(input.normal);
    return float4(1.0f, 1.0f, 1.0f, 1.0f);

    // //Set diffuse color of material
    // float4 diffuse = difColor;
    // // diffuse = float4(0.5f, 0.5f, 0.5f, 1.0f);

    // if(input.normal.g < 0.0f)
    //     return float4(0.0f, 0.0f, 0.0f, 1.0f);

    // //If material has a diffuse texture map, set it now
    // if(hasTexture == true)
    //     diffuse = ObjTexture.Sample( ObjSamplerState, input.texcoord );
    // // diffuse.g = abs((diffuse.g * input.worldPos.x * 0.05))%1;


    // //If material has a normal map, we can set it now
    // if(hasNormMap == true)
    // {
    //     //Load normal from normal map
    //     float4 normalMap = ObjNormMap.Sample( ObjSamplerState, input.texcoord );

    //     //Change normal map range from [0, 1] to [-1, 1]
    //     normalMap = (2.0f*normalMap) - 1.0f;

    //     //Make sure tangent is completely orthogonal to normal
    //     input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);
    //     //Create the biTangent
    //     float3 biTangent = cross(input.normal, input.tangent);

    //     //Create the "Texture Space"
    //     float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

    //     //Convert normal from normal map to texture space and store in input.normal
    //     input.normal = normalize(mul(normalMap, texSpace));
    // }

    // float3 finalColor = float3(0.0f, 0.0f, 0.0f);

    // if(any(light.att)) {
    //     //Point Light Version
    //     //Create the vector between the light position and the pixels position
    //     float3 lightToPixelVec = light.pos - input.worldPos;
        

    //     //Find the distance between the light pos and pixel pos
    //     float d = length(lightToPixelVec);

    //     //Create the ambient light
    //     float3 finalAmbient = diffuse * light.ambient;

    //     //If the pixel is too far from the light, just return just ambient
    //     if(d>light.range)
    //         return float4(finalAmbient, diffuse.a);

    //     //Turn lightToPixelVec into a unit length vector describing the pixels direction from light to pixel
    //     lightToPixelVec /= d;

    //     //Calculate how much light the pixel gets by the angle in which the light strikes the pixels surface
    //     float howMuchLight = dot(lightToPixelVec, input.normal);
        
    //     //If light is triking the front side of the pixel
    //     if(howMuchLight > 0.0f) {
    //         //Add light to the final color of the pixel
    //         finalColor += howMuchLight * diffuse * light.diffuse;

    //         //Calculate Light's Falloff factor
    //         finalColor /= light.att[0] + (light.att[1]*d) + (light.att[2]*d*d);
            
    //         //If this is a spotlight... only have light in direction
    //         if(light.cone > 0.0f) {
    //             //Calculate falloff from center to edge of pointlight cone
    //             finalColor *= pow(max(dot(-lightToPixelVec, light.dir), 0.0f), light.cone);
    //         }
    //     }

    //     //Make sure the values are between 1 and 0, and add the ambient
    //     finalColor = saturate(finalColor + finalAmbient);
    // } else {
    //     //Directional Light Version
    //     finalColor = diffuse * light.ambient;
    //     finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);
    // }
    // //REturn the final color
    // // return float4(input.normal, diffuse.a);
    // return float4(finalColor, diffuse.a);

}