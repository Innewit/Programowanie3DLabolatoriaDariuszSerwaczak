#version 420

in vec3 vertexColor;

out vec4 FragColor;

layout(std140, binding = 0) uniform Modifier {
 float strength;
 vec3  color;
};

layout(std140, binding=1) uniform Transformations {
 vec2 scale;
 vec2 translation;
 mat2 rotation;
};

void main() {
    FragColor = vec4(vertexColor * strength * color, 1.0f);
}
