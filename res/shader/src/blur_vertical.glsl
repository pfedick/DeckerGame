// Vertical Blur Fragment Shader
#version 450

layout(binding = 0) uniform sampler2D inputTexture;
layout(binding = 1, std140) uniform BlurParams {
    float blurStrength;  // 0.0 - 1.0
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
    
    color += texture(inputTexture, texCoord) * weights[0];
    
    for(int i = 1; i < 9; ++i) {
        vec2 offset = vec2(0.0, texelSize.y * i * blurStrength);
        color += texture(inputTexture, texCoord + offset) * weights[i];
        color += texture(inputTexture, texCoord - offset) * weights[i];
    }
    
    fragColor = color;
}