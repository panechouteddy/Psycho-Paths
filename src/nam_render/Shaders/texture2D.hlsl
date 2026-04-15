#define MAX_TEXTURES 512

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
    int hasTexture;
};

cbuffer cbProj : register(b1)
{
    float4x4 proj;
    float time;
}

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORDS;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORDS;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    
    float4 worldPos = mul(float4(vin.PosL.xy, 0, 1), gWorld);
    vout.PosH = mul(worldPos, proj);
    vout.Color = vin.Color;
    vout.UV = vin.UV;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 albedo = pin.Color;
    
    if(hasTexture == 1)
    {
        albedo *= gTexture.Sample(gSampler, pin.UV);
    }
    
    return albedo;
}


