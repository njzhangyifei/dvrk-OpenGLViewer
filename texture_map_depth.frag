#version 330 core

in vec4 gl_FragCoord;
in vec2 TexCoords;
out vec4 gl_FragColor;
out float gl_FragDepth;


uniform sampler2D texture_color_sampler;
uniform sampler2D texture_depth_sampler;

uniform bool use_depth;
uniform bool use_distortion;

uniform vec4 camera_center_focus;
uniform vec2 image_size;
uniform vec3 distortion_radial;
uniform vec2 distortion_tangential;

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
    vec2 mapped_tex_coord = vec2(
        (1 - texture_scale_width) * 0.5  + texture_scale_width  * TexCoords.x,
        (1 - texture_scale_height) * 0.5 + texture_scale_height * TexCoords.y
    );
//        ((1 + texture_scale_height) * 0.5 - texture_scale_height * TexCoords.y)
//    use_distortion = false;
    if (use_distortion) {
        vec2 image_coord = mapped_tex_coord * image_size;
        vec2 normalized_center = (camera_center_focus.xy / camera_center_focus.zw);
        normalized_center.y = (image_size.y/camera_center_focus.w)-normalized_center.y;
        vec2 normalized_coord = ((image_coord / camera_center_focus.zw) - (normalized_center) );
        float r_2 = dot(normalized_coord, normalized_coord);
        float radial_distort = (
            1 + distortion_radial.x * r_2
              + distortion_radial.y * r_2 * r_2
              + distortion_radial.z * r_2 * r_2 * r_2
        );
        vec2 tangential_distort = vec2(
            2 * distortion_tangential.x * normalized_coord.x * normalized_coord.y +
                distortion_tangential.y * ((r_2) + 2 * (normalized_coord.x * normalized_coord.x)),
            2 * distortion_tangential.y * normalized_coord.x * normalized_coord.y +
                distortion_tangential.x * ((r_2) + 2 * (normalized_coord.y * normalized_coord.y))
        );
        vec2 distorted_normalized_coord = radial_distort * normalized_coord + tangential_distort;
        vec2 distorted_coord = distorted_normalized_coord * camera_center_focus.zw +
                                normalized_center * camera_center_focus.zw;
//        camera_center_focus.xy;
        float r_2_diff= dot(distorted_normalized_coord, distorted_normalized_coord) - r_2;
        mapped_tex_coord = distorted_coord / image_size;

//        vec2 distance = (TexCoords.xy - camera_center_focus.xy / camera_center_focus.zw);
        vec2 distance = (normalized_coord);
////        vec2 distance = normalized_coord;
        float distance_2 = dot(distance, distance);
        gl_FragDepth = 0.0f;
        gl_FragColor = colormap(distance_2 * 100);
        return;
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
