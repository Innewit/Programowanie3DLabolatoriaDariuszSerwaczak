//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Application/utils.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // A vector containing the x,y,z vertex coordinates for the square (base of the house) and the triangle (roof of the house).
    std::vector<GLfloat> vertices = {
        -0.5f, -0.25f, 0.0f, 0.9f, 0.4f, 0.3f,  // Bottom left corner of the square
         0.5f, -0.25f, 0.0f, 0.6f, 0.9f, 0.3f,  // Bottom right corner of the square
         0.5f,  0.25f, 0.0f, 0.6f, 0.4f, 0.9f,  // Top right corner of the square
        -0.5f,  0.25f, 0.0f, 0.9f, 0.4f, 0.3f,  // Top left corner of the square
         0.0f,  0.75f, 0.0f, 0.6f, 0.4f, 0.7f   // Top of the roof
    };

    // Creating uniform buffer
    GLuint u_buffer_handle;
    glGenBuffers(1, &u_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, u_buffer_handle);

    // Define uniform parameters for pixel color and strength
    float strength = 0.8f; // replace with appropriate value
    float color[4] = {0.8f, 0.5f, 0.9f, 1.0f}; // replace with appropriate values

    // Load strength and color
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 4 * sizeof(float), color);

    // Create transformation buffer
    GLuint ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 48, NULL, GL_STATIC_DRAW); // 48 bytes
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);

    // Load transformations to buffer
    float theta = 1.0*glm::pi<float>()/6.0f;
    auto cs = std::cos(theta);
    auto ss = std::sin(theta);
    glm::mat2 rot{cs,ss,-ss,cs};
    glm::vec2 trans{0.0,  -0.25};
    glm::vec2 scale{0.5, 0.5};

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec2), glm::value_ptr(scale));
    glBufferSubData(GL_UNIFORM_BUFFER, 8, sizeof(glm::vec2), glm::value_ptr(trans));
    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec2), glm::value_ptr(rot[0]));
    glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(glm::vec2), glm::value_ptr(rot[1]));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    #if __APPLE__
        // Get the location of the uniform block
        GLuint blockIndex = glGetUniformBlockIndex(program, "Transformations");

        // Set the binding point for the uniform block
        GLuint bindingPoint = 1;
        glUniformBlockBinding(program, blockIndex, bindingPoint);
    #endif

    // Creating indices buffer
    GLuint i_buffer_handle;
    std::vector<GLushort> indices_buffer = {0, 1, 2, 4, 3};
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_buffer.size() * sizeof(GLushort), indices_buffer.data(), GL_STATIC_DRAW);

    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);

    // This indicates that the data for attribute 0 (position) should be read from a vertex buffer.
    glEnableVertexAttribArray(0);
    // and this specifies how the position data is layout in the buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));

    // This indicates that the data for attribute 1 (color) should be read from a vertex buffer.
    glEnableVertexAttribArray(1);
    // and this specifies how the color data is layout in the buffer.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    // Draw the square (base of the house)
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, nullptr);
    // Draw the triangle (roof of the house)
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)(3 * sizeof(GLushort)));
    glBindVertexArray(0);
}
