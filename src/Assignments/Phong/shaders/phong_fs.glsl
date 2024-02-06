#version 460

layout(location=0) out vec4 vFragColor;
const int MAX_POINT_LIGHTS=24;

#if __VERSION__ > 410
layout(std140, binding=0) uniform Modifiers {
#else
    layout(std140) uniform Color {
    #endif
    vec4  Kd;
};

struct PointLight {
    vec3 position_in_view_space;
    vec3 color;
    float intensity;
    float radius;
} ;

layout(std140, binding=3) uniform Lights {
    vec3 ambient;
    uint n_p_lights;
    PointLight p_light[MAX_POINT_LIGHTS];
};

in vec2 vertex_texcoords;
in vec3 vertex_normals_in_vs;
in vec3 vertex_coords_in_vs;

uniform sampler2D map_Kd;

void main() {
    vFragColor.a = Kd.a;
    vFragColor.rgb = Kd.rgb*ambient;
}