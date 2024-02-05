#version 460

layout(location=0) out vec4 vFragColor;

#if __VERSION__ > 410
layout(std140, binding=0) uniform Modifiers {
#else
    layout(std140) uniform Color {
    #endif
    vec4  Kd;
};

in vec2 vertex_texcoords;
in vec3 vertex_normals_in_vs;
in vec3 vertex_coords_in_vs;

uniform sampler2D map_Kd;

void main() {
    vFragColor = Kd;
    vFragColor.rgb = abs(vertex_coords_in_vs);
}