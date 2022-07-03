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
    int type;
    float4 ambient;
    float4 diffuse;
};

cbuffer cbPerFrame : register(b0)
{
    int numberLights;
    Light lights[16];
};

cbuffer cbPerObject : register(b1)
{
    float4x4 WVP;
    float4x4 World;

    float4 difColor;
    bool hasTexture;
    bool hasNormMap;
};

float3 DirLight(Light inlight, float3 normal, float4 diffuse);
float3 PointLight(Light inlight, float3 normal, float4 diffuse, float4 worldPos);
float3 SpotLight(Light inlight, float3 normal, float4 diffuse, float4 worldPos);

Texture2D ObjTexture;
Texture2D ObjNormMap;
SamplerState ObjSamplerState;

VOut VShader(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT)
{
    VOut output;

    output.position = mul(position, WVP);

    output.worldPos = mul(position, World);
    // return output;
    
    output.normal = mul(normal, World);

    output.texcoord = texcoord;

    output.tangent = mul(tangent, World);

    return output;
}


float4 PShader(VOut input) : SV_TARGET
{
    input.normal = normalize(input.normal);

    //Set diffuse color of material
    float4 diffuse = difColor;
    // return diffuse;

    //WHY DID COMMENTING THIS OUT FIX IT??? I CANT FIGURE OUT WHY THIS WAS HERE INITIALLY
    // if(input.normal.g < 0.0f)
    //     return float4(0.0f, 0.0f, 0.0f, 1.0f);

    //If material has a diffuse texture map, set it now
    // if(hasTexture == true)
    //     diffuse = ObjTexture.Sample( ObjSamplerState, input.texcoord );
    // diffuse.g = abs((diffuse.g * input.worldPos.x * 0.05))%1;


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

    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    // return float4(abs(input.normal), 1.0f);
    // return lights[0].ambient;
    return float4(PointLight(lights[0], input.normal, diffuse, input.worldPos), 1.0f);

    for(int i = 0; i < numberLights; i++) {
        int type = lights[i].type;
        if(type == 0)
            finalColor += DirLight(lights[i], input.normal, diffuse);
        else if(type == 1)
            finalColor += PointLight(lights[i], input.normal, diffuse, input.worldPos);
        else if(type == 2)
            finalColor += SpotLight(lights[i], input.normal, diffuse, input.worldPos);
    }

    
    //REturn the final color
    return saturate(float4(finalColor, diffuse.a));

}


float3 DirLight(Light inlight, float3 normal, float4 diffuse) {
    float3 color = float3(0.0f, 0.0f, 0.0f);
    color = diffuse * inlight.ambient;
    color += saturate(dot(inlight.dir, normal) * inlight.diffuse * diffuse);
    return color;
}
float3 PointLight(Light inlight, float3 normal, float4 diffuse, float4 worldPos) {
        //Point Light Version
        float3 color = float3(0.0f, 0.0f, 0.0f);
        //Create the vector between the light position and the pixels position
        float3 lightToPixelVec = inlight.pos - worldPos;
        
        //Find the distance between the light pos and pixel pos
        float d = length(lightToPixelVec);

        //Create the ambient light
        float3 finalAmbient = diffuse * inlight.ambient;

        //If the pixel is too far from the light, just return just ambient
        if(d>inlight.range)
            return float4(finalAmbient, diffuse.a);

        //Turn lightToPixelVec into a unit length vector describing the pixels direction from light to pixel
        lightToPixelVec /= d;

        //Calculate how much light the pixel gets by the angle in which the light strikes the pixels surface
        float howMuchLight = dot(lightToPixelVec, normal);
        
        //If light is triking the front side of the pixel
        if(howMuchLight > 0.0f) {
            //Add light to the final color of the pixel
            color += howMuchLight * diffuse * inlight.diffuse;

            //Calculate Light's Falloff factor
            color /= inlight.att[0] + (inlight.att[1]*d) + (inlight.att[2]*d*d);
        }

        //Make sure the values are between 1 and 0, and add the ambient
        color = saturate(color + finalAmbient);
        return color;
}

float3 SpotLight(Light inlight, float3 normal, float4 diffuse, float4 worldPos) {
        //SpotLight Version
        float3 color = float3(0.0f, 0.0f, 0.0f);
        //Create the vector between the light position and the pixels position
        float3 lightToPixelVec = inlight.pos - worldPos;
        

        //Find the distance between the light pos and pixel pos
        float d = length(lightToPixelVec);

        //Create the ambient light
        float3 finalAmbient = diffuse * inlight.ambient;

        //If the pixel is too far from the light, just return just ambient
        if(d>inlight.range)
            return float4(finalAmbient, diffuse.a);

        //Turn lightToPixelVec into a unit length vector describing the pixels direction from light to pixel
        lightToPixelVec /= d;

        //Calculate how much light the pixel gets by the angle in which the light strikes the pixels surface
        float howMuchLight = dot(lightToPixelVec, normal);
        
        //If light is triking the front side of the pixel
        if(howMuchLight > 0.0f) {
            //Add light to the final color of the pixel
            color += howMuchLight * diffuse * inlight.diffuse;

            //Calculate Light's Falloff factor
            color /= inlight.att[0] + (inlight.att[1]*d) + (inlight.att[2]*d*d);
            
            if(inlight.cone > 0.0f) {
                //Calculate falloff from center to edge of pointlight cone
                color *= pow(max(dot(-lightToPixelVec, inlight.dir), 0.0f), inlight.cone);
            }
        }

        //Make sure the values are between 1 and 0, and add the ambient
        color = saturate(color + finalAmbient);
        return color;
}