// Horizontal Blur Fragment Shader
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

    // PRE-MULTIPLY ALPHA: Wichtig für korrekten Blur bei Transparenz
    // Wir wandeln Straight Alpha in Premultiplied Alpha um, damit (0,0,0,0) nicht als "Schwarz" gewichtet wird
    vec4 center = texture(inputTexture, uv);
    center.rgb *= center.a;
    color += center * weights[0];
    
    for(int i = 1; i < 9; ++i) {
        vec2 offset = vec2(texelSize.x * i * blurStrength, 0.0);
        
        vec4 col1 = texture(inputTexture, uv + offset);
        col1.rgb *= col1.a;
        color += col1 * weights[i];
        
        vec4 col2 = texture(inputTexture, uv - offset);
        col2.rgb *= col2.a;
        color += col2 * weights[i];
    }
    
    // UN-PREMULTIPLY ALPHA: Zurück zu Straight Alpha für SDL Renderer
    // Dies verhindert die dunklen Ränder beim Blending
    if(color.a > 0.0) {
        fragColor = vec4(color.rgb / color.a, color.a);
    } else {
        fragColor = vec4(0.0);
    }
}