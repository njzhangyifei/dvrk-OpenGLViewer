#version 330 core

in vec4 gl_FragCoord;
in vec2 TexCoords;
out vec4 gl_FragColor;
out float gl_FragDepth;

uniform sampler2D texture_color_sampler;
uniform sampler2D texture_depth_sampler;

uniform bool use_depth;

void main()
{
    vec4 tex_color = texture(texture_color_sampler, TexCoords).rgba;
    float frag_z = gl_FragCoord.z;
    if (use_depth) {
        float texture_depth = texture(texture_depth_sampler, TexCoords).x;
        frag_z = texture_depth;
    }
    gl_FragDepth = frag_z;
    if (!use_depth) {
        if (frag_z < 0.81 && frag_z > 0.79) {
            // 0.8
            tex_color = vec4(1, 1, 0, 1);
        }
    }
    gl_FragColor = tex_color;
}
