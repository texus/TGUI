Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input
{
    float4 Color : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
};

float4 main(Input input) : SV_Target0
{
    return Texture.Sample(Sampler, input.TexCoord) * input.Color;
}
