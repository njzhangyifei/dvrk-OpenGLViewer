#version 330 core

in vec2 TexCoords;
out vec4 color;
//out float gl_FragDepth;

uniform sampler2D texture_color_sampler;
uniform sampler2D texture_depth_sampler;

void main()
{
    vec3 texture_color = texture(texture_sampler, TexCoords).rgb;
    color = vec4(texture_color, 1);
}
