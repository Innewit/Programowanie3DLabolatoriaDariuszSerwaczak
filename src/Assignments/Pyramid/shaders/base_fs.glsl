#version 460

flat in vec3 vertexColor;

out vec4 FragColor;

#if defined(__APPLE__)
    layout(std140, binding=1) uniform Modifier
#else
    layout(std140) uniform Modifier
#endif
{
 float strength;
 vec3  color;
};

void main() {
    // Create a random noise
    vec3 rand_noise = vec3(float(gl_PrimitiveID) + 1) / 2;

    FragColor = vec4(vertexColor * strength * color * rand_noise, 1.0f);
}
