#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D texture_sampler;

void main()
{
//    vec3 texture_color = texture(texture_sampler, TexCoords).rgb;
//    color = vec4(vec3(depthValue), 1.0); // orthographic
//    color = vec4(texture_color, 1);
    color = vec4(0, 0, 1, 1);
}
