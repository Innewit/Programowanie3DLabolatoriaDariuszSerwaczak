#version 460

layout(location = 0) in vec4 a_vertex_position;
layout(location = 1) in vec3 a_vertex_color;

#if defined(__APPLE__)
    layout(std140, binding=1) uniform Transformations
#else
    layout(std140) uniform Transformations
#endif
{
 vec2 scale;
 vec2 translation;
 mat2 rotation;
};

out vec3 vertexColor;

void main() {
    vertexColor = a_vertex_color;
    gl_Position.xy = rotation*(scale*a_vertex_position.xy)+translation;
    gl_Position.zw = a_vertex_position.zw;
}
