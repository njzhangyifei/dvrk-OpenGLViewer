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
    color = texture(texture_color_sampler, TexCoords).rgba;
    if (use_depth) {
        float texture_depth = texture(texture_depth_sampler, TexCoords).x;
        gl_FragDepth = texture_depth;
//        gl_FragDepth = texture_depth;
//        gl_FragDepth = 0.1;
//        color = vec4(gl_FragDepth, gl_FragDepth, gl_FragDepth, 1.0);
    } else {
        gl_FragDepth = gl_FragCoord.z;
    }
}
