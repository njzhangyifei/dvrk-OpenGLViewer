#version 330 core

in vec4 gl_FragCoord;
in vec2 TexCoords;
out vec4 color;

out float gl_FragDepth;

uniform sampler2D texture_color_sampler;
uniform sampler2D texture_depth_sampler;

uniform bool use_depth;

void main()
{
    vec3 texture_color = texture(texture_color_sampler, TexCoords).rgb;
    if (use_depth) {
        float texture_depth = texture(texture_depth_sampler, TexCoords).x;
        gl_FragDepth = texture_depth;
        color = vec4(gl_FragDepth, gl_FragDepth, gl_FragDepth, 1.0);
//        color = vec4(texture_color, 1);
    } else {
        color = vec4(texture_color, 1);
        gl_FragDepth = gl_FragCoord.z;
    }
}
