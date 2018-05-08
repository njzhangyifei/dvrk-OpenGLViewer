#version 330 core

in vec4 gl_FragCoord;
in vec2 TexCoords;
out vec4 gl_FragColor;
out float gl_FragDepth;

uniform sampler2D texture_color_sampler;
uniform sampler2D texture_depth_sampler;

uniform bool use_depth;
uniform float texture_scale_height;
uniform float texture_scale_width;

void main()
{
    vec2 mapped_tex_coord = vec2(
        (1 - texture_scale_width) * 0.5  + texture_scale_width  * TexCoords.x,
        (1 - texture_scale_height) * 0.5 + texture_scale_height * TexCoords.y
    );
    vec4 tex_color = texture(texture_color_sampler, mapped_tex_coord).rgba;
    if (tex_color.a < 0.01) {
        discard;
    }
    float frag_z = gl_FragCoord.z;
    if (use_depth) {
        float texture_depth = texture(texture_depth_sampler, mapped_tex_coord).x;
        frag_z = texture_depth;
    }
//    if (!use_depth) {
//        if (frag_z < 0.81 && frag_z > 0.79) {
//            // 0.8
//            tex_color = vec4(1, 1, 0, 1);
//        }
//    }
    gl_FragDepth = frag_z;
    gl_FragColor = tex_color;
}
