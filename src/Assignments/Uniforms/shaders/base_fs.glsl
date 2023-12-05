#version 420

in vec3 vertexColor;

out vec4 FragColor;

layout(std140, binding = 0) uniform Modifier {
 float strength;
 vec3  color;
};

void main() {
    FragColor = vec4(vertexColor * strength * color, 1.0f);
}
