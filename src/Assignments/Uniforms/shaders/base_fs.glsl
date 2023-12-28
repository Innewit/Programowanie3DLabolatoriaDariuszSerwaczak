#version 460

in vec3 vertexColor;

out vec4 FragColor;

#if _Windows == 1
    layout(std140) uniform Modifier
#else
    layout(std140, binding=0) uniform Modifier
#endif
{
 float strength;
 vec3  color;
};

void main() {
    FragColor = vec4(vertexColor * strength * color, 1.0f);
}
