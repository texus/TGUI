#version 450

// Colors in TGUI are sRGB values. When rendering to an sRGB target, the GPU converts the output from linear to sRGB,
// so in that case the colors have to be converted to linear first.
layout(constant_id = 0) const bool convertToLinear = false;

layout(set = 0, binding = 0) uniform texture2D tex;
layout(set = 1, binding = 0) uniform sampler texSampler;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(sampler2D(tex, texSampler), fragTexCoord) * fragColor;
    if (convertToLinear)
    {
        const vec3 linearLow = outColor.rgb / 12.92;
        const vec3 linearHigh = pow((outColor.rgb + 0.055) / 1.055, vec3(2.4));
        outColor.rgb = mix(linearHigh, linearLow, lessThanEqual(outColor.rgb, vec3(0.04045)));
    }
}
