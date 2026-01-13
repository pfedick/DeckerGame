// Vertical Blur Fragment Shader
#version 450

// Texture Input -> Set 2, Binding 0
layout(set = 2, binding = 0) uniform sampler2D inputTexture;

// Params -> Set 3, Binding 0
// Hinweis: Wir nutzen Binding 0, das müssen wir im C++ Code bei PushGPUFragmentUniformData beachten!
layout(set = 3, binding = 0, std140) uniform BlurParams {
    float blurStrength;  // 0.0 - 1.0
    float _padding;
    vec2 texelSize;
};

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 fragColor;

void main() {
    vec4 color = vec4(0.0);
    float weights[9] = float[](
        0.147761, 0.118318, 0.094879, 0.075484, 0.060626,
        0.048297, 0.038771, 0.030001, 0.024177
    );
    // Fallback: Bildschirmkoordinaten -> UV (0..1) aus gl_FragCoord,
    // um fehlende TexCoord-Interpolation zu umgehen
    vec2 uv = gl_FragCoord.xy * texelSize;

    color += texture(inputTexture, uv) * weights[0];
    
    for(int i = 1; i < 9; ++i) {
        vec2 offset = vec2(0.0, texelSize.y * i * blurStrength);
        color += texture(inputTexture, uv + offset) * weights[i];
        color += texture(inputTexture, uv - offset) * weights[i];
    }
    fragColor = color;
}
