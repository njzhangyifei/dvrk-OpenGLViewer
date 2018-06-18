#version 330 core

in vec4 gl_FragCoord;
in vec2 TexCoords;
out vec4 gl_FragColor;
out float gl_FragDepth;


uniform sampler2D texture_color_sampler;
uniform sampler2D texture_depth_sampler;
uniform sampler2D texture_distortion_sampler;

uniform bool use_depth;
uniform bool use_distortion;

uniform vec2 image_size;

uniform float texture_scale_height;
uniform float texture_scale_width;


// MATLAB jet colormap
float colormap_red(float x) {
    if (x < 0.7) {
        return 4.0 * x - 1.5;
    } else {
        return -4.0 * x + 4.5;
    }
}

float colormap_green(float x) {
    if (x < 0.5) {
        return 4.0 * x - 0.5;
    } else {
        return -4.0 * x + 3.5;
    }
}

float colormap_blue(float x) {
    if (x < 0.3) {
       return 4.0 * x + 0.5;
    } else {
       return -4.0 * x + 2.5;
    }
}

vec4 colormap(float x) {
    float r = clamp(colormap_red(x), 0.0, 1.0);
    float g = clamp(colormap_green(x), 0.0, 1.0);
    float b = clamp(colormap_blue(x), 0.0, 1.0);
    return vec4(r, g, b, 1.0);
}

void main()
{
    vec2 tex_coord = TexCoords;
    vec2 mapped_tex_coord = tex_coord;

    if (use_distortion){
        vec3 distorted_target = texture(texture_distortion_sampler, tex_coord).rgb;
        if (distorted_target.z < 0){
            discard;
        }
        distorted_target.y = 1 - distorted_target.y;
        mapped_tex_coord = distorted_target.xy;
    }

    float frag_z = gl_FragCoord.z;
    if (mapped_tex_coord.x < 0  || mapped_tex_coord.x >= 1  || mapped_tex_coord.y < 0 || mapped_tex_coord.y >= 1) {
        discard;
    }
    vec4 tex_color = texture(texture_color_sampler, mapped_tex_coord).rgba;
    if (tex_color.a < 0.01) {
        discard;
    }
    if (use_depth) {
        float texture_depth = texture(texture_depth_sampler, mapped_tex_coord).x;
        frag_z = texture_depth;
    }

    gl_FragDepth = frag_z;
    gl_FragColor = tex_color;
//    gl_FragColor = colormap(frag_z);
}
