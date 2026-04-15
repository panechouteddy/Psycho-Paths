#define MAX_LIGHTS 64
#define MAX_TEXTURES 512
#define DIRECTIONNAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

float3 reflect(float3 v, float3 n)
{
    return v - 2.0 * dot(v, n) * n;
}

float average(float3 v)
{
    return (v.x + v.y + v.z) / 3.f;
}

struct Light
{
    uint type;
    float3 color;

    float3 direction;
    uint id;
    
    float3 position;
    float pad2;
    
    float range;
    float beginAngleRad;
    float endAngleRad;
    float intensity;

};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
    float4x4 invGWorld;
    int hasTexture;
    int isSkinned;
};

cbuffer cbViewProj : register(b1)
{
    float4x4 view;
    float4x4 proj;
    float3 cameraPos;
    float time;

    float deltaTime;
};

cbuffer cbLight : register(b2)
{
    Light lights[MAX_LIGHTS];
    
    float3 ambientColor;
    uint lightsCount;
}

cbuffer cbMaterial : register(b3)
{
    float3 albedoMat;
    float roughness;
    float metalness;
}

cbuffer cbSkinning : register(b4)
{
    float4x4 BoneTransforms[256];
};

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORDS;
    float3 NormalL : NORMAL;
    uint4 BoneIndices : BLENDINDICES;
    float4 BoneWeights : BLENDWEIGHTS;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORDS;
    float3 NormalH : NORMAL;
    float3 WorldPos : POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
   
    float4 worldPos = float4(0, 0, 0, 0);
    
    if(false)
    {
        float4 pos = float4(vin.PosL, 1.0f);

        float totalWeight = vin.BoneWeights.x + vin.BoneWeights.y + vin.BoneWeights.z + vin.BoneWeights.w;
        if (totalWeight > 0.0f)
        {
            vin.BoneWeights /= totalWeight;
        }
        
        float4 skinnedPos =
        vin.BoneWeights.x * mul(pos, BoneTransforms[vin.BoneIndices.x]) +
        vin.BoneWeights.y * mul(pos, BoneTransforms[vin.BoneIndices.y]) +
        vin.BoneWeights.z * mul(pos, BoneTransforms[vin.BoneIndices.z]) +
        vin.BoneWeights.w * mul(pos, BoneTransforms[vin.BoneIndices.w]);
        
        worldPos = mul(skinnedPos, gWorld);
        vout.PosH = mul(worldPos, mul(view, proj));

        float3 skinnedNormal = pos;
        //vin.BoneWeights.x * mul(vin.NormalL, (float3x3) BoneTransforms[vin.BoneIndices.x]) +
        //vin.BoneWeights.y * mul(vin.NormalL, (float3x3) BoneTransforms[vin.BoneIndices.y]) +
        //vin.BoneWeights.z * mul(vin.NormalL, (float3x3) BoneTransforms[vin.BoneIndices.z]) +
        //vin.BoneWeights.w * mul(vin.NormalL, (float3x3) BoneTransforms[vin.BoneIndices.w]);
        
        float3 worldNormal = mul(skinnedNormal, (float3x3) gWorld);
        worldNormal = normalize(worldNormal);
        vout.NormalH = worldNormal;
        vout.Color = float4(vin.BoneIndices.x / 100.0f, 0, 0, 1);
    }
    else
    {
        worldPos = mul(float4(vin.PosL, 1), gWorld);
        vout.PosH = mul(worldPos, mul(view, proj));
        vout.NormalH = normalize(mul(vin.NormalL, (float3x3) gWorld));
        vout.Color = vin.Color;
    }
   
  
    vout.WorldPos = worldPos.xyz;
    vout.UV = vin.UV;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float3 pixNormal = normalize(pin.NormalH);
    float3 pixToView = normalize(cameraPos - pin.WorldPos);

    float3 accumulatedLights = float3(0, 0, 0);

    float3 albedo = pin.Color.rgb * albedoMat;

    if (hasTexture == 1)
    {
        albedo *= gTexture.Sample(gSampler, pin.UV).rgb;
    }

    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metalness);
    float3 diffuseColor = albedo * (1.0 - metalness);

    for (int i = 0; i < lightsCount; i++)
    {
        float3 lightColor = lights[i].color * lights[i].intensity;
        float3 lightDir = float3(0, 0, 0);

        if (lights[i].type == DIRECTIONNAL_LIGHT)
        {
            lightDir = normalize(-lights[i].direction);
        }
        else if (lights[i].type == POINT_LIGHT)
        {
            lightDir = normalize(lights[i].position - pin.WorldPos);

            float distance = length(lights[i].position - pin.WorldPos);
            float attenuation = saturate(1.0 - distance / lights[i].range);

            lightColor *= attenuation;
        }
        else if (lights[i].type == SPOT_LIGHT)
        {
            lightDir = normalize(lights[i].position - pin.WorldPos);

            float theta = dot(lightDir, normalize(-lights[i].direction));
            float diff = lights[i].beginAngleRad - lights[i].endAngleRad;

            float intensity = saturate((theta - lights[i].endAngleRad) / diff);

            float distance = length(lights[i].position - pin.WorldPos);
            float attenuation = saturate(1.0 - distance / lights[i].range);

            lightColor *= attenuation * intensity;
        }

        float NdotL = max(dot(pixNormal, lightDir), 0.0);

        float3 diffuse = diffuseColor * lightColor * NdotL;

        float3 H = normalize(lightDir + pixToView);

        float specPower = lerp(4.0, 128.0, 1.0 - roughness);

        float spec = pow(max(dot(pixNormal, H), 0.0), specPower);

        float3 specular = F0 * spec * lightColor * NdotL;
        
        accumulatedLights += diffuse + specular;
    }

    float3 finalColor = accumulatedLights + albedo * ambientColor;

    return float4(finalColor, 1.0);
}


